#pragma once
#include <stdlib.h>
#include <string.h>
#include <libpcap/include/pcap.h>

#define ETHER_HDRLEN 14
#define IP_HDRLEN 20
#define VLAN_LEN 4
#define ETHER_ADDR_LEN 6

#define IP_VERSION_4 4
#define IP_PROTOCAL_TCP 6

/* TCP flags */
#define TH_FIN     0x01
#define TH_SYN     0x02
#define TH_RST     0x04
#define TH_PUSH    0x08
#define TH_ACK     0x10
#define TH_URG     0x20
#define TH_ECNECHO 0x40 /* ECN Echo */
#define TH_CWR     0x80 /* ECN Cwnd Reduced */

#define ETHERTYPE_8021Q 0x8100
#define IPV4TYPE 0x0800

struct VlanHeader {
    uint16_t rezerve;
    uint16_t length_type;
};

struct EtherHeader {
    uint8_t ether_dhost[ETHER_ADDR_LEN];
    uint8_t ether_shost[ETHER_ADDR_LEN];
    uint16_t ether_length_type;
};

struct IpHeader {
    uint8_t ip_vhl; /* header length(4bytes unit), version */
#define IP_V(ip_header) (((ip_header)->ip_vhl & 0xf0) >> 4)
#define IP_HL(ip_header) ((ip_header)->ip_vhl & 0x0f)
    uint8_t ip_tos; /* type of service */
    uint16_t ip_len; /* total length */
    uint16_t ip_id; /* identification */
    uint16_t ip_off; /* fragment offset field */
#define IP_DF 0x4000 /* dont fragment flag */
#define IP_MF 0x2000 /* more fragments flag */
#define IP_OFFMASK 0x1fff /* mask for fragmenting bits */
    uint8_t ip_ttl; /* time to live */
    uint8_t ip_p; /* protocol */
    uint16_t ip_sum; /* checksum */
    uint32_t ip_src,ip_dst; /* source and dest address */
};

struct TcpHeader {
    uint16_t port_src;
    uint16_t port_dst;
    uint32_t sn;
    uint32_t ack;
    uint16_t hlen_flags;
#define TCP_HL(tcp_header) (((ntohs((tcp_header)->hlen_flags) >> 12) & 0x0f) * 4)
#define TCP_FLAGS(tcp_header) (ntohs((tcp_header)->hlen_flags) & 0x3f)
    uint16_t win_size;
    uint16_t check_sum;
    uint16_t surgent_pointer;
};
