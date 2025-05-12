#include "args.hpp"
#include "sha256.hpp"
#include <iostream>
#include <sstream>  // std::ostringstream

/**
 * Run checksum mode.
 */
static void checksum(const std::string &in, std::string &out) {
    SHA256(in).sha256(out);
}

/**
 * Run MAC mode.
 */
static void mac(const std::string &in, std::string &out, Arguments &a) {
    std::string message = a.key();
    message.append(in);

    SHA256(message).sha256(out);
}

/**
 * Validate given MAC hash.
 */
static int validate(const std::string &in, Arguments &a) {
    std::string temp = "";
    mac(in, temp, a);

    return temp.compare(a.mac_message()) == 0 ? 0 : 1;
}

/**
 * Run Length Extension Attack.
 */
static void length_extension_attack(const std::string &in, Arguments &a) {
    std::string out_hash = "";
    std::string out_message = "";
    SHA256(in)
        .sha256_extend_unsafe(a.message_extension(), a.mac_message(), out_hash);
    std::cout << out_hash << std::endl;
    SHA256(in)
        .print_mblocks_unsafe(a.key_len(), a.message_extension());
}

int main(int argc, char **argv) {
    Arguments a{argc, argv};
    if (!a.validate_args()) {
        exit(EXIT_FAILURE);
    }

    std::string buffer = "";
    std::ostringstream sstream;
    sstream << std::cin.rdbuf();
    std::string input = sstream.str();

    if (a.checksum()) {
        checksum(input, buffer);
        std::cout << buffer << std::endl;
    } else if (a.mac()) {
        mac(input, buffer, a);
        std::cout << buffer << std::endl;
    } else if (a.validate()) {
        return validate(input, a);
    } else if (a.length_extension_attack()) {
        length_extension_attack(input, a);
    } else {
        a.print_help();
        return 1;
    }
    return 0;
}

/* main.cpp */
