#include "args.hpp"

bool Arguments::checksum() { return checksum_; }
bool Arguments::mac() { return mac_; }
bool Arguments::validate() { return validate_; }
bool Arguments::length_extension_attack() { return length_extension_attack_; }
bool Arguments::key_is_set() { return key_is_set_; }
bool Arguments::mac_message_is_set() { return mac_message_is_set_; }
bool Arguments::key_len_is_set() { return key_len_is_set_; }
bool Arguments::message_extension_is_set() { return message_extension_is_set_; }

const std::string &Arguments::key() const { return key_; }
const std::string &Arguments::mac_message() const { return mac_message_; }
int Arguments::key_len() { return key_len_; }
const std::string &Arguments::message_extension() const {
    return message_extension_;
}

void Arguments::debug() {
    std::cout << "Checksum: " << checksum_ << std::endl
              << "Mac: " << mac_ << std::endl
              << "Validate: " << validate_ << std::endl
              << "LEA: " << length_extension_attack_ << std::endl
              << "Key: " << key_ << std::endl
              << "MAC Message: " << mac_message_ << std::endl
              << "Key length: " << key_len_ << std::endl
              << "Message extension: " << message_extension_ << std::endl;
}

bool Arguments::validate_args() {
    // missing arguments when -s option is used
    if (mac_ && !key_is_set_) {
        goto err_missing_oper;
    }
    // missing arguments when -v option is used
    if (validate_ && (!key_is_set_ || !mac_message_is_set_)) {
        goto err_missing_oper;
    }
    // missing arguments when -e option is used
    if (length_extension_attack_ &&
        (!key_len_is_set_ || !mac_message_is_set_ || !message_extension_is_set_)) {
        goto err_missing_oper;
    }

    // exclusive -c/-s/-v/-e
    if (checksum_ &&
        (mac_ || validate_ || length_extension_attack_)) {
        goto err_exclusive;
    }
    if (mac_ &&
        (checksum_ || validate_ || length_extension_attack_)) {
        goto err_exclusive;
    }
    if (validate_ &&
        (mac_ || checksum_ || length_extension_attack_)) {
        goto err_exclusive;
    }
    if (length_extension_attack_ &&
        (mac_ || validate_ || checksum_)) {
        goto err_exclusive;
    }

    return true;

err_missing_oper:
    std::cerr << "Missing options!" << std::endl;
    print_help();
    return false;
err_exclusive:
    std::cerr << "Cannot run multiple modes at once!" << std::endl;
    print_help();
    return false;
}

void Arguments::print_help() {
    std::cout << "Usage:\n"
                 "./kry [-c] [-s -k KEY] [-v -k KEY -m MAC_MESSAGE]\n"
                 "      [-e -n NUM -a TEXT -m HASH_VALUE]\n"
                 "\n"
                 "Modes:\n"
                 "  -c  Generate a checksum of the input on STDIN.\n"
                 "  -s  Generate a MAC of the input on STDIN. Requires `-k` flag to be set.\n"
                 "  -v  Validate MAC of STDIN with the given MAC. Requires `-k` and `-m` flags to be set.\n"
                 "  -e  Perform a length extension attack. Requires `-a`, `-m`, and `-n` flags to be set.\n"
                 "\n"
                 "Parameter flags:\n"
                 "  -k      A key to encrypt a message and generate a MAC hash.\n"
                 "  -m      MAC hash value.\n"
                 "  -n      The size of the key used during length extension attack.\n"
                 "  -a      Text to append during length extension attack."
              << std::endl;
}

/* args.cpp */
