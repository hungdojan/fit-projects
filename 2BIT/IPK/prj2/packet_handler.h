/**
 * Header file with declaration of pcap callback function.
 * This file also contains some additional constants used in packet_handler.c file.
 *
 * This source code serves as submission for second project of class IPK at FIT, BUT 2021/2022.
 *
 * @file    packet_handler.h
 * @author  Hung Do
 * @date    04/04/2022
 */
#ifndef _DEFAULT_SOURCE
    #define _DEFAULT_SOURCE
#endif
#ifndef _PACKET_HANDLER_H_
#define _PACKET_HANDLER_H_

#include <pcap.h>

/**
 * Declaration of pcap callback function
 */
void packet_handler(u_char *, const struct pcap_pkthdr *, const u_char *);

/**
 * Update pcap value.
 * @param handle New instance of pcap handle
 */
void set_handle(pcap_t *handle);

#endif // _PACKET_HANDLER_H_
