/**
 * Header file with declarations of function used for displaying each part of a header.
 *
 * This source code serves as submission for second project of class IPK at FIT, BUT 2021/2022.
 *
 * @file    header_display.h
 * @author  Hung Do
 * @date    04/04/2022
 */
#ifndef _DEFAULT_SOURCE
    #define _DEFAULT_SOURCE
#endif
#ifndef _HEADER_DISPLAY_H_
#define _HEADER_DISPLAY_H_

#include <stdlib.h>
#include <stdint.h>

#include <netinet/tcp.h>        // struct tcphdr
#include <netinet/udp.h>        // struct udphdr
#include <netinet/ip_icmp.h>    // struct icmphdr

/**
 * Print arrival time of the packet.
 *
 * @param timestamp[in] Packet arrival's timestamp
 */
void print_timestamp(struct timeval timestamp);

/**
 * Prints frame information.
 * @param time_of_arrival[in] Packet's time of arrival
 * @param frame_size[in]      Frame size in bytes
 */
void print_frame_info(struct timeval time_of_arrival, unsigned frame_size);

/**
 * Extracts and prints MAC addresses from ether header of packet.
 *
 * @param bytes[in] Packet's frame
 * @param size[in]  Number of bytes of the frame
 */
void print_eth_header(const u_char *bytes, size_t size);


/**
 * Extracts and prints IP addresses from ip header of packet.
 * Out buffers must be at least INET6_ADDRSTRLEN big (46 bytes).
 *
 * @param bytes[in]       Packet's frame
 * @param size[in]        Number of bytes of the frame
 * @param ethertype[in]   EtherType of the packet
 */
void print_ip_header(const u_char *bytes, size_t size, uint16_t ethertype);

/**
 * Prints ICMP and ICMPv6 header of the packet.
 *
 * @param icmp[in] ICMP header
 */
void print_icmp_header(const struct icmphdr *icmp);

/**
 * Prints TCP header of the packet.
 *
 * @param tcp[in] TCP header
 */
void print_tcp_header(const struct tcphdr *tcp);

/**
 * Prints UDP header of the packet.
 *
 * @param udp[in] UDP header
 */
void print_udp_header(const struct udphdr *udp);

/**
 * Prints ARP header of the packet.
 *
 * @param bytes[in] Packet's buffer
 */
void print_arp_header(const u_char *bytes);

/**
 * Dumps data buffer as hex.
 *
 * @param bytes[in] Packet's buffer
 * @param size[in]  Buffer size
 */
void data_dump(const u_char *bytes, size_t size);

#endif // _HEADER_DISPLAY_H_
