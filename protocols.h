#include <stdint.h>

#pragma once

#define ETH_ARP_PROT 0x0806
#define ETH_IPV4_PROT 0x0800
#define MAC_ADR_SIZE 6
#define IP_ADR_SIZE 4
#define ETHERNET_TYPE 1
#define IPV4_TYPE 8

#define SWAP_WORD(_x) (uint16_t)((((_x) << 8) | ((_x) >> 8)))
#define SWAP_DWORD(_x) (uint32_t)((SWAP_WORD((_x) & 0xFFFF) << 16) | (SWAP_WORD((_x) >> 16)))
#define SET_BIT(_val, _bit) ((_val) | (1 << (_bit)))
#define CLEAR_BIT(_val, _bit) ((_val) & ~(1 << (_bit)))

#pragma pack(push)
#pragma pack(1)

struct mac_adr
{
    uint8_t mac_adr[MAC_ADR_SIZE];
};

struct ip_adr
{
    union ip_adr_union
    {
        uint8_t ip_adr[IP_ADR_SIZE];
        uint32_t val32;
    } data;
};

struct ethernet_header
{
    struct mac_adr dest;
    struct mac_adr source;
    uint16_t eather_type;
};

struct arp
{
    uint16_t hardware_type;
    uint16_t protocol_type;
    uint8_t hardware_length;
    uint8_t protocol_length;
    uint16_t operation;
    struct mac_adr mac_sender;
    struct ip_adr ip_sender;
    struct mac_adr mac_target;
    struct ip_adr ip_target;
};

union flags
{
    struct
    {
        uint16_t offset : 13;
        uint16_t flag_00 : 1;
        uint16_t flag_DF : 1;
        uint16_t flag_MF : 1;
    } ip_flags;

    uint16_t val16;
};

struct ip
{
    uint8_t ver_ihl;
    uint8_t differentiated_services_code_point_ecn;
    uint16_t total_length;
    uint16_t identification;
    union flags flags_fragment_offset;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;
    struct ip_adr ip_sender;
    struct ip_adr ip_target;
};

struct ethernet_typeII_frame
{
    struct ethernet_header header;
    union data
    {
        struct arp arp_protocol;
        struct ip ip_protocol;
        uint8_t val8[1500];
    } payload;
    uint32_t crc_checksum;
};

extern struct mac_adr mac_adr_sender;
extern struct mac_adr mac_adr_target;
extern struct mac_adr mac_adr_broadcast;
extern struct ip_adr ip_adr_sender;
extern struct ip_adr ip_adr_target;

int make_arp(uint8_t *tx_buf);

struct ip *make_ip(uint8_t *tx_buf);

struct ethernet_header *make_ethernet_header(uint8_t *tx_buf);

void print_ip(struct ip_adr *ip, char *name);

void print_mac(struct mac_adr *mac);

#pragma pack(pop)
