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

#define DEBUG_MODE

void mem_dump(void *ptr, size_t mem_size, FILE *file_ptr)
{
    if (!ptr)
        return;
    uint8_t *arr = (uint8_t *)ptr;
    if (arr)
    {
        for (int i = 0; i < mem_size; i++)
        {
            fprintf(file_ptr, "%02X ", arr[i]);
            if ((i + 1) % 16 == 0)
                fprintf(file_ptr, "\n");
        }
    }
    fprintf(file_ptr, "\n");
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

int read_packet(int file_descr, uint8_t *tx_buf, FILE *file_ptr)
{

    if (file_descr < 0 || !tx_buf || !file_ptr)
    {
        perror("read_packet");
        return -1;
    }
    bool check = false;

    for (int i = 0; i < 10; i++)
    {
        ssize_t bytes = read(file_descr, tx_buf, 100);
        if (bytes <= 0)
        {
            perror("read");
            continue;
        }
        printf("\ni = %d\nGot frame: %d bytes \n", i, bytes);
        fprintf(file_ptr, "\ni = %d\nGot frame: %d bytes \n", i, bytes);
        mem_dump(tx_buf, (ssize_t)bytes, file_ptr);
        if (bytes == 42)
            return 0;
    }

    return 0;
}

void read_file(FILE *file_ptr)
{
    file_ptr = fopen("/home/alina/Documents/data.txt", "r");
    char arr[100];
    while (fgets(arr, 100, file_ptr))
    {
        printf("%s", arr);
    }
}

int main()
{
    uint8_t txbuf[1000] = {0};
    FILE *file_ptr = fopen("/home/alina/Documents/data.txt", "w");
    if (!file_ptr)
    {
        perror("file_ptr");
        exit;
    }

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
    read_packet(file_descr, txbuf, file_ptr);
    tap_close(file_descr);
    fclose(file_ptr);

    read_file(file_ptr);
}