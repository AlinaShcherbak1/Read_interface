#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/if_tun.h>
#include <net/if.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>

#include "protocols.h"

#define DEBUG_MODE

void mem_dump(void *ptr, size_t mem_size)
{
    if (!ptr)
        return;
    uint8_t *arr = (uint8_t *)ptr;
    if (arr)
    {
        for (int i = 0; i < mem_size; i++)
        {
            printf("%02X ", arr[i]);
            if ((i + 1) % 16 == 0)
                printf("\n");
        }
    }
    printf("\n");
}

int tap_close(int file_descr)
{
    return (close(file_descr));
}

int tap_open(char *iface_name)
{
    int file_descr = open("/dev/net/tun", O_RDWR);
    if (file_descr <= 0)
    {
        exit(EXIT_FAILURE);
        return -1;
    }

    struct ifreq ifr = {0};
    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
    strncpy(ifr.ifr_name, iface_name, IFNAMSIZ - 1);
    ifr.ifr_name[IFNAMSIZ - 1] = '\0';
    if (ioctl(file_descr, TUNSETIFF, &ifr) != 0)
    {
        tap_close(file_descr);
        return -1;
    }
    return file_descr;
}

void print_ifaces_ip(uint16_t eth_type, struct ip_adr *ip_sender, struct ip_adr *ip_target, ssize_t bytes, int i, uint8_t *tx_buf)
{
    switch (eth_type)
    {
    case ETH_ARP_PROT:
    {
        printf("\n\nGot an ARP packet\n");
        break;
    }
    case ETH_IPV4_PROT:
    {
        printf("\n\nGot an IP packet\n");
        break;
    }
    default:
        exit;
    }
    print_ip(ip_sender, "Sender");
    print_ip(ip_target, "Target");
    printf("\npacket = %d\nGot frame: %d bytes \n", i, bytes);
    mem_dump(tx_buf, (ssize_t)bytes);
}

int read_packet(int file_descr, uint8_t *tx_buf)
{

    if (file_descr < 0 || !tx_buf)
    {
        perror("read_packet");
        return -1;
    }

    for (int i = 0; i < 20; i++)
    {
        ssize_t bytes = read(file_descr, tx_buf, 100);
        if (bytes <= 0)
        {
            perror("read");
            continue;
        }
        uint16_t Ethernet_type = (tx_buf[12] << 8 | tx_buf[13]);

        switch (Ethernet_type)
        {
        case ETH_ARP_PROT:
        {
            struct ip_adr ip_sender, ip_target;
            ip_sender.data.val32 = SWAP_DWORD((uint32_t)tx_buf[28] << 24 | (uint32_t)tx_buf[29] << 16 | (uint32_t)tx_buf[30] << 8 | (uint32_t)tx_buf[31]);
            ip_target.data.val32 = SWAP_DWORD((uint32_t)tx_buf[38] << 24 | (uint32_t)tx_buf[39] << 16 | (uint32_t)tx_buf[40] << 8 | (uint32_t)tx_buf[41]);
            print_ifaces_ip(Ethernet_type, &ip_sender, &ip_target, bytes, i, tx_buf);
            break;
        }
        case ETH_IPV4_PROT:
        {
            struct ip_adr ip_sender, ip_target;
            ip_sender.data.val32 = SWAP_DWORD((uint32_t)tx_buf[26] << 24 | (uint32_t)tx_buf[27] << 16 | (uint32_t)tx_buf[28] << 8 | (uint32_t)tx_buf[29]);
            ip_target.data.val32 = SWAP_DWORD((uint32_t)tx_buf[30] << 24 | (uint32_t)tx_buf[31] << 16 | (uint32_t)tx_buf[32] << 8 | (uint32_t)tx_buf[33]);
            print_ifaces_ip(Ethernet_type, &ip_sender, &ip_target, bytes, i, tx_buf);
            break;
        }
        default:
            break;
        }
    }

    return 0;
}

int main()
{
    uint8_t txbuf[1000] = {0};

#ifndef DEBUG_MODE
    char iface_name[20];
    printf("Enter dev: ");
    scanf("%s20", iface_name);
#else
    char iface_name[] = "iface2";
#endif // DEBUG_MODE

    int file_descr = tap_open(iface_name);
    if (file_descr < 0)
    {
        perror("file_descr failed");
        return 1;
    }
    read_packet(file_descr, txbuf);
    tap_close(file_descr);
}