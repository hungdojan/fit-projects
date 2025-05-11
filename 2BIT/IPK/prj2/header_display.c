/**
 * Definition of functions declared in header_display.h.
 *
 * This source code serves as submission for second project of class IPK at FIT, BUT 2021/2022.
 *
 * @file    header_display.c
 * @author  Hung Do
 * @date    04/04/2022
 */
#include "header_display.h"

#include <stdio.h>          // printf
#include <arpa/inet.h>      // inet_ntoa

#include <netinet/ether.h>  // struct ethhdr
#include <netinet/ip.h>     // struct iphdr
#include <netinet/ip6.h>    // struct ip6_hdr
#include <net/if_arp.h>         // struct arphdr

#include <string.h>
#include <math.h>    // ceil
#include <time.h>    // time_t, struct tm

// padding for headers
#define PADDING "%-22s"

/**
 * Prints out MAC address format.
 * @param msg   Message to print before MAC format
 * @param addr  Six-byte array contain MAC address
 */
static void print_mac_addr(const char *msg, const unsigned char addr[6]) {
    // at least 18 bytes are needed (including '\0'
    printf(PADDING "%.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n", msg,
             addr[0], addr[1], addr[2],
             addr[3], addr[4], addr[5]);
}

void print_timestamp(struct timeval timestamp) {
    // store seconds in struct time_t
    time_t *time = (time_t *)(&timestamp.tv_sec);

    // use localtime to convert time_t to time
    struct tm *my_time = localtime(time);
    char buffer[100] = {0,};
    char timezone_buffer[100] = {0,};
    strftime(buffer, 100, "%FT%T", my_time);

    // create string of timezone offset
    // parameter %z doesn't contain colon separator, which is needed for RFC3339
    // so it needs to be manually added
    strftime(timezone_buffer, 100, "%z", my_time);
    int index = strlen(timezone_buffer);
    for (int i = 0; i < 2; i++) {
        timezone_buffer[index] = timezone_buffer[index-1];
        --index;
    }
    timezone_buffer[index] = ':';

    printf(PADDING "%s.%ld%s\n","Arrival time:", buffer, timestamp.tv_usec / 100, timezone_buffer);
}

void print_frame_info(struct timeval time_of_arrival, unsigned frame_size) {
    puts("--------------- Frame info ---------------");
    print_timestamp(time_of_arrival);
    printf(PADDING "%d bytes\n", "Frame length:", frame_size);
}

void print_eth_header(const u_char *bytes, const size_t size) {
    // extract ether header from buffer of bytes
    struct ethhdr *eth_header = (struct ethhdr *)(bytes);
    (void)size;

    // display MAC addresses
    puts("--------------- Ethernet II --------------");
    print_mac_addr("Src MAC: ", eth_header->h_source);
    print_mac_addr("Dst MAC: ", eth_header->h_dest);
    printf(PADDING "0x%.4x\n", "Type:", ntohs(eth_header->h_proto));
}

void print_ip_header(const u_char *bytes, const size_t size, const uint16_t ethertype) {
    (void)size;
    // buffer for storing formatted IP address
    char addr_buffer[INET6_ADDRSTRLEN] = {0,};

    puts("------------ Internet Protocol -----------");
    if (ethertype == ETHERTYPE_IP) {
        // IPv4
        struct iphdr *ip = (struct iphdr *)(bytes + sizeof(struct ethhdr));
        inet_ntop(AF_INET, &ip->saddr, addr_buffer, INET_ADDRSTRLEN);
        printf(PADDING "%s\n", "Src IP:", addr_buffer);
        inet_ntop(AF_INET, &ip->daddr, addr_buffer, INET_ADDRSTRLEN);
        printf(PADDING "%s\n", "Drc IP:", addr_buffer);
    } else if (ethertype == ETHERTYPE_IPV6) {
        // IPv6
        struct ip6_hdr *ip = (struct ip6_hdr *)(bytes + sizeof(struct ethhdr));
        inet_ntop(AF_INET6, &ip->ip6_src, addr_buffer, INET6_ADDRSTRLEN);
        printf(PADDING "%s\n", "Src IP:", addr_buffer);
        inet_ntop(AF_INET6, &ip->ip6_dst, addr_buffer, INET6_ADDRSTRLEN);
        printf(PADDING "%s\n", "Drc IP:", addr_buffer);
    }
    // Unknown types are ignored
}

void print_icmp_header(const struct icmphdr *icmp) {
    puts("----- Internet Control Message Protocol ------");
    printf(PADDING "%d\n", "Type:", icmp->type);
    printf(PADDING "%d\n", "Code:", icmp->code);
    printf(PADDING "%u\n", "Checksum:", ntohs(icmp->checksum));
}

void print_tcp_header(const struct tcphdr *tcp) {
    puts("----- Transmission Control Protocol ------");
    printf(PADDING "%d\n", "Src port:", ntohs(tcp->th_sport));
    printf(PADDING "%d\n", "Dst port:", ntohs(tcp->th_dport));
    printf(PADDING "%u\n", "Sequence num:", tcp->seq);
    printf(PADDING "%u\n", "Acknowledgment num:", tcp->ack);
    printf(PADDING "0x%.4x\n", "Flags:", ntohs(tcp->th_flags));
}

void print_udp_header(const struct udphdr *udp) {
    puts("---------- User Diagram Protocol ---------");
    printf(PADDING "%d\n", "Src port:", ntohs(udp->uh_sport));
    printf(PADDING "%d\n", "Dst port:", ntohs(udp->uh_dport));
    printf(PADDING "%d\n", "Length:", udp->len);
}

void print_arp_header(const u_char *bytes) {
    struct arphdr *arp = (struct arphdr *)(bytes + sizeof(struct ethhdr));
    puts("------ Address Resolution Protocol -------");
    printf(PADDING "%u\n", "Hardware type:", ntohs(arp->ar_hrd));
    printf(PADDING "0x%.4x\n", "Protocol type:", ntohs(arp->ar_pro));
    printf(PADDING "%u\n", "Hardware size:", arp->ar_hrd);
    printf(PADDING "%u\n", "Protocol size:", arp->ar_pln);
    printf(PADDING "%u\n", "Opcode:", ntohs(arp->ar_op));

    // get addresses
    unsigned char *address_buffer = (unsigned char *)(bytes + sizeof(struct ethhdr) + sizeof(struct arphdr));
    char buffer[INET6_ADDRSTRLEN];

    // sender MAC address
    print_mac_addr("Sender MAC address:", address_buffer);
    address_buffer += 6;

    // sender IP address
    inet_ntop(AF_INET, address_buffer, buffer, INET6_ADDRSTRLEN);
    printf(PADDING "%s\n", "Sender IP address:", buffer);
    address_buffer += 4;

    // target MAC address
    print_mac_addr("Target MAC address:", address_buffer);
    address_buffer += 6;

    // sender IP address
    inet_ntop(AF_INET, address_buffer, buffer, INET6_ADDRSTRLEN);
    printf(PADDING "%s\n", "Target IP address:", buffer);
}

void data_dump(const u_char *bytes, const size_t size) {
    puts("-------------- Data Dump -----------------");
    const size_t width = 16;
    const size_t height = ceil((double)size / 16.0);
    size_t top = 0;
    for (; top < height; top++) {
        printf("0x%-7.4zx", top*width);
        // hex code
        for (size_t left = 0; left < width; left++) {
            if (left)       printf(" ");
            if (left == 8)  printf(" ");
            if (top*width + left < size)
                printf("%.2x", bytes[top*width + left]);
            else
                printf("  ");
        }

        // add padding
        printf("%3s", "");

        // printable characters
        for (size_t left = 0; left < 16 && top*width +left < size; left++) {
            if (left == 8)  printf(" ");
            if (bytes[top*width + left] >= 32 && bytes[top*width + left] < 128)
                putchar(bytes[top*width + left]);
            else
                putchar('.');
        }
        putchar('\n');
    }
    puts("");
}
/* header_extract.c */
