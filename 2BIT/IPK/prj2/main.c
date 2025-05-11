/**
 * Main file of packet sniffer.
 * This file initializes pcap handle and runs pcap loop.
 *
 * This source code serves as submission for second project of class IPK at FIT, BUT 2021/2022.
 *
 * @file    main.c
 * @author   Hung Do
 * @date    04/04/2022
 */

/*
 * _DEFAULT_SOURCE macro is defined due to strange data types used in pcap.h header
 * Author: user862787
 * Date: Mar 13, 2013
 * Web source: https://stackoverflow.com/questions/15393905/c-pcap-library-unknown-types-error
 */
#ifndef _DEFAULT_SOURCE
    #define _DEFAULT_SOURCE
#include <pcap/pcap.h>
#endif

#include <stdio.h>
#include <string.h>

#include "packet_handler.h"
#include "argumets.h"

#define BUFFER_SIZE 1024

/**
 * Display list of active devices.
 */
int display_active_devices() {
    pcap_if_t *alldevsp;
    char errbuf[BUFFER_SIZE] = {0,};

#ifdef DEBUG
    puts("Search for available devices ...");
#endif

    // load list of available devices
    if (pcap_findalldevs(&alldevsp, errbuf)) {
        fprintf(stderr, "Error while finding devices:\n%s", errbuf);
        return 1;
    }

#ifdef DEBUG
    puts("Scan completed.");
#endif
    // print list of available devices
    for (pcap_if_t *device = alldevsp; device; device = device->next) {
        printf("%s\n", device->name);
    }
    return 0;
}

/**
 * Run packet sniffing.
 */
int process_packets() {
    char errbuf[BUFFER_SIZE] = {0,};
    char interface_name[BUFFER_SIZE];

    // get interface
    args_get_interface(interface_name, BUFFER_SIZE);

    // initialize handle
    // pcap_t *handle = pcap_open_live(interface_name, 65536, 1, 10, errbuf);
    pcap_t *handle = pcap_create(interface_name, errbuf);

    if (handle == NULL) {
        fprintf(stderr, "Error occurred while opening %s interface.\nError message: %s\n", interface_name, errbuf);
        return 1;
    }
    // set some options before activation
    pcap_set_timeout(handle, 10);
    pcap_set_buffer_size(handle, 65536);
    pcap_activate(handle);
#ifdef DEBUG
    printf("Device %s successfully initializes\n", interface_name);
#endif

    set_handle(handle);
    // process indefinite number of packets
    // loop is broke in packet_handler callback function
    pcap_loop(handle, -1, packet_handler, NULL);
#ifdef DEBUG
    puts("Packet processing ended with no error.");
#endif
    pcap_close(handle);
    return 0;
}

int main(int argc, char *argv[]) {
    // argument processing
    init_args_t();
    enum load_results result = load_args(argc, (char * const *)argv);

    switch (result) {
        case LOAD_SUCCESS:
            return process_packets();

        // some error happened while loading arguments
        case LOAD_ERROR:
            fprintf(stderr, "Error occurred while loading arguments\n");
            return 1;

        // interface device was not defined
        case LOAD_NO_INTERFACE:
            return display_active_devices();

        // -h|--help was called
        case LOAD_HELP:
        default:
            break;
    }
    return 0;
}

/* main.c */
