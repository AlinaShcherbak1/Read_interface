#include "protocols.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

struct mac_adr mac_adr_sender = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
struct mac_adr mac_adr_target = {0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
struct mac_adr mac_adr_broadcast = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
struct ip_adr ip_adr_sender = {192, 168, 1, 2};
struct ip_adr ip_adr_target = {192, 168, 2, 2};

int make_arp(uint8_t *tx_buf)
{
    if (!tx_buf)
        return 0;

    struct arp *p_arp = (struct arp *)&tx_buf[14];

    p_arp->hardware_type = ETHERNET_TYPE;
    p_arp->protocol_type = IPV4_TYPE;
    p_arp->hardware_length = MAC_ADR_SIZE;
    p_arp->protocol_length = IP_ADR_SIZE;
    p_arp->operation = 0x01; // request
    p_arp->mac_sender = mac_adr_sender;
    p_arp->ip_sender = ip_adr_sender;
    p_arp->mac_target = mac_adr_target;
    p_arp->ip_target = ip_adr_target;

    return sizeof(struct arp);
}

struct ip *make_ip(uint8_t *tx_buf)
{
    if (!tx_buf)
        return NULL;

    struct ip *p_ip = (struct ip *)&tx_buf[14];
    p_ip->ver_ihl = (4 << 4) | 5;
    p_ip->differentiated_services_code_point_ecn = 0x00;
    p_ip->total_length = 20;
    p_ip->identification = 0x1234;
    p_ip->flags_fragment_offset.val16 = 0xE00F; // 1110 0000 0000 1111
    p_ip->ttl = 128;
    p_ip->protocol = 17;
    p_ip->checksum = 9;
    p_ip->ip_sender = ip_adr_sender;
    p_ip->ip_target = ip_adr_target;

    return p_ip;
}

struct ethernet_header *make_ethernet_header(uint8_t *tx_buf)
{
    if (!tx_buf)
        return NULL;

    struct ethernet_header *p_eth_header = (struct ethernet_header *)&tx_buf[0]; // p_eth_header = (struct ethernet_header*) tx_buf; // ��������� �� 0-�� ������� �������
    p_eth_header->dest = mac_adr_target;
    p_eth_header->source = mac_adr_sender;
    p_eth_header->eather_type = SWAP_WORD(ETH_ARP_PROT);

    return p_eth_header;
}

void print_ip(struct ip_adr *ip, char *name)
{
    if (ip)
        printf("IP %s               : %u.%u.%u.%u\n", name, ip->data.ip_adr[0], ip->data.ip_adr[1], ip->data.ip_adr[2], ip->data.ip_adr[3]);
}

void print_mac(struct mac_adr *mac)
{
    if (mac)
        printf("MAC Sender              : %02X:%02X:%02X:%02X:%02X:%02X\n", mac->mac_adr[0], mac->mac_adr[1], mac->mac_adr[2], mac->mac_adr[3], mac->mac_adr[4], mac->mac_adr[5]);
}
