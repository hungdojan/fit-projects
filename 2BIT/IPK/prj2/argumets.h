/**
 * Header file of processing program arguments.
 * Creates some basic API to get results of argument processing.
 *
 * This source code serves as submission for second project of class IPK at FIT, BUT 2021/2022.
 *
 * @file    arguments.h
 * @author  Hung Do
 * @date    04/04/2022
 */
#ifndef _ARGUMETS_H_
#define _ARGUMETS_H_

#include <stdlib.h> // size_t

enum load_results {
    LOAD_SUCCESS,
    LOAD_ERROR,
    LOAD_NO_INTERFACE,
    LOAD_HELP
};

/**
 * Initializes argument structure.
 */
void init_args_t();

/**
 * Process program arguments.
 *
 * @param argv[in] Vector of arguments
 * @param argc[in] Argument count
 * @return Argument processing results
 */
enum load_results load_args(int argc, char * const *argv);

/**
 * Returns interface value from argument structure
 *
 * @param buffer[out]     Buffer to store function output 
 * @param buffer_siza[in] Output buffer size
 * @return Non-zero value when error occures.
 */
int args_get_interface(char *buffer, size_t buffer_size);

/**
 * Retuns loaded port.
 * Output port will be in range 0-65536. -1 will be returned when user 
 * didn't specified port.
 *
 * @return Port number, -1 when not specified.
 */
int args_get_port();


/**
 * Returns ICMP flag's value.
 * 
 * @return true if user defined ICMP value.
 */
int args_isicmp();

/**
 * Returns TCP flag's value.
 * 
 * @return true if user defined TCP value.
 */
int args_istcp();

/**
 * Returns UDP flag's value.
 * 
 * @return true if user defined UDP value.
 */
int args_isudp();

/**
 * Returns ARP flag's value.
 * 
 * @return true if user defined ARP value.
 */
int args_isarp();

/**
 * Returns number of packets to display.
 *
 * @return Number of values to display.
 */
unsigned args_get_nof_packets();

/**
 * Print out argument structure content.
 * Used for debugging purposes.
 */
void args_print_structure();

#endif // _ARGUMETS_H_
