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
        return -1;

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

int read_packet(int file_descr, uint8_t *tx_buf)
{
    if (!file_descr || !tx_buf)
        return -1;

    while (1)
    {
        int bytes = read(file_descr, tx_buf, 1000);
        printf("\nGot frame: %d bytes \n", bytes);
        mem_dump(tx_buf, bytes);
    }

    return 0;
}

int main()
{
    uint8_t txbuf[1000] = {0};
    char iface_name[] = "iface2";
    printf("hello world!\n");
    int file_descr = tap_open(iface_name);
    read_packet(file_descr, txbuf);
}