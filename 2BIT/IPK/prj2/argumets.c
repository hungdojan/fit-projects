/**
 * Definition of argument.h functions.
 * This file contains private global instance of structure "struct args_t" that hold
 * information of processed arguments. It's a simulation of SINGLETON; there is only
 * one instance of this structure in whole program.
 *
 * This source code serves as submission for second project of class IPK at FIT, BUT 2021/2022.
 *
 * @file    arguments.c
 * @author  Hung Do
 * @date    04/04/2022
 */
#include <string.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>     // exit
#include "argumets.h"

#define ICMP_BIT 0
#define TCP_BIT  1
#define UDP_BIT  2
#define ARP_BIT  3
#define INT_BUFFER_SIZE 1024

struct args_t {
    char interface[INT_BUFFER_SIZE];
    int port;
    char flags;
    char flags_enable;
    unsigned nof_packets;
};

// define global variable
static struct args_t g_args = {
    .interface = {0,},      /* interface name */
    .port = -1,             /* port filter */
    .flags = 0,             /* flags for packet filtering */
    .flags_enable = 0,      /* when user doesn't specify any flag*/
    .nof_packets = 1        /* number of packets to display */
};

/**
 * Prints program usage.
 */
static void print_help() {
    printf("Packet sniffer\n"
           "./ipk-sniffer [-i interface_name | --interface interface_name] {-p port} {[--tcp|-t] [--udp|-u] [--arp] [--icmp]} {-n num}\n"
           "\n"
           "Options:\n"
           "   -h|--help                           Display this help message\n"
           "   -i|--interface interface_name       Set interface device to sniff from. When no interface device is defined\n"
           "                                           program displays list of available devices and terminates.\n"
           "   -p port                             Define port to filter packets. Program will only display packets where\n"
           "                                           its source or destination port equals to user defined port.i\n"
           "   -t|--tcp                            Display TCP packets.\n"
           "   -u|--udp                            Display UDP packets.\n"
           "   --icmp                              Display ICMP and ICMPv6 packets.\n"
           "   --arp                               Display ARP packets.\n"
           "   -n num                              Display *num* packets.\n"
    );
}

void init_args_t() {
    // empties buffer and sets default values
    g_args.interface[0] = '\0';
    g_args.port = -1;
    g_args.flags = 0;
    g_args.flags_enable = 0;
    g_args.nof_packets = 1;
}

enum load_results load_args(int argc, char * const *argv) {
    // setup
    static const struct option long_options[] = {
            {"help", no_argument, NULL, 'h'},
            {"interface", optional_argument, NULL, 'i'},
            {"tcp", no_argument, NULL, 't'},
            {"udp", no_argument, NULL, 'u'},
            {"icmp", no_argument, NULL, 'I'},
            {"arp", no_argument, NULL, 'a'},
            {NULL, 0, NULL, 0}
    };
    static const char *short_options = "hi::p:tuIan:";

    enum load_results return_value = LOAD_SUCCESS;
    char c;
    int option_index = 0, interface_set = 0;

    // arguments processing
    while((c = getopt_long(argc, (char *const *) argv, short_options, long_options, &option_index)) != -1) {
        switch(c) {
            case 'h':       // --help
                print_help();
                return LOAD_HELP;
            case 'i':       // --interface
                // get interface name
                if (optind < argc && argv[optind][0] != '-') {
                    // store last loaded interface
                    snprintf(g_args.interface, INT_BUFFER_SIZE, "%s", argv[optind]);
                    interface_set = 1;
                }
                break;
            case 'p':       // port
            {
                // convert argument to int and check port range
                int port = (int) strtol(optarg, NULL, 10);

                if (port >= 0 && port <= 65536) {
                    g_args.port = port;
                } else {    // port is out of range 0..65 536
                    fprintf(stderr, "Warning: port %d is out of range, filter is ignored\n", port);
                    g_args.port = -1;
                }
            } // case 'p'
                break;
            case 't':       // --tcp
                g_args.flags_enable = 1;
                g_args.flags |= (1 << TCP_BIT);
                break;
            case 'u':       // --udp
                g_args.flags_enable = 1;
                g_args.flags |= (1 << UDP_BIT);
                break;
            case 'I':       // --icmp
                g_args.flags_enable = 1;
                g_args.flags |= (1 << ICMP_BIT);
                break;
            case 'a':       // --arp
                g_args.flags_enable = 1;
                g_args.flags |= (1 << ARP_BIT);
                break;
            case 'n':       // number of packets
            {
                unsigned nof_packets = strtol(optarg, NULL, 10);
                // TODO: get error code -> errno??
                if (!nof_packets) {
                    fprintf(stderr, "Error: -n argument must be unsigned value greater than 0\n");
                    return_value = LOAD_ERROR;

                } else {
                    g_args.nof_packets = nof_packets;
                }
                break;
            } // case 'n'
            default:    // other undefined options are ignored
                fprintf(stderr, "Warning: Invalid use of -%c option. This option has not definition.\n", c);
                break;
        } // switch(c)
    } // while ((c = getopt_long()) != -1)
#ifdef DEBUG
    puts("Arguments loaded successfully");
#endif
    if (return_value != LOAD_ERROR && !interface_set)
        return_value = LOAD_NO_INTERFACE;
    return return_value;
}

int args_get_interface(char *buffer, size_t buffer_size) {
    // check for buffer size
    // halts copy when destination buffer is too small
    if (buffer_size <= strlen(g_args.interface))
        return 1;

    strncpy(buffer, g_args.interface, buffer_size);
    return 0;
}

int args_get_port() {
    return g_args.port;
}

int args_isicmp() {
    return g_args.flags & (1 << ICMP_BIT) || !g_args.flags_enable;
}

int args_istcp() {
    return g_args.flags & (1 << TCP_BIT) || !g_args.flags_enable;
}

int args_isudp() {
    return g_args.flags & (1 << UDP_BIT) || !g_args.flags_enable;
}

int args_isarp() {
    return g_args.flags & (1 << ARP_BIT) || !g_args.flags_enable;
}

unsigned args_get_nof_packets() {
    return g_args.nof_packets;
}

void args_print_structure() {
    printf("interface: %s\n"
           "port: %d\n"
           "TCP: %d\n"
           "UDP: %d\n"
           "ICMP: %d\n"
           "ARP: %d\n"
           "number of packets: %d\n",
           g_args.interface, args_get_port(), args_istcp(), args_isudp(), args_isicmp(), args_isarp(), args_get_nof_packets());
}
/* argumets.c */
