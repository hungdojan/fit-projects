/**
 * Header file defining the `Arguments` class.
 *
 * This class processes program arguments and does some input validations.
 *
 * @author  Hung Do (xdohun00@stud.fit.vutbr.cz)
 * @date    29.03.2023
 * @file    args.hpp
 */
#ifndef _ARGS_HPP_
#define _ARGS_HPP_
#include <iostream>     // std::cerr
#include <string>       // std::string
#include <unistd.h>     // getopt

/**
 * @brief Class that manage command line arguments.
 */
class Arguments {
  protected:
    bool checksum_;                 /** -c option is set */
    bool mac_;                      /** -s option is set */
    bool validate_;                 /** -v option is set */
    bool length_extension_attack_;  /** -e option is set */
    bool key_is_set_;               /** -k option is set */
    bool mac_message_is_set_;       /** -m option is set */
    bool key_len_is_set_;           /** -n option is set */
    bool message_extension_is_set_; /** -a option is set */

    std::string key_;               /** -k option value */
    std::string mac_message_;       /** -m option value */
    int key_len_;                   /** -n option value */
    std::string message_extension_; /** -a option value */

  public:
    /** Checksum flag */
    bool checksum();
    /** MAC mode flag */
    bool mac();
    /** Validate MAC flag */
    bool validate();
    /** length_extension_attack flag */
    bool length_extension_attack();
    /** Key value flag */
    bool key_is_set();
    /** MAC message flag */
    bool mac_message_is_set();
    /** Key length flag */
    bool key_len_is_set();
    /** Extending string flag */
    bool message_extension_is_set();

    const std::string &key() const;
    const std::string &mac_message() const;
    int key_len();
    const std::string &message_extension() const;

    Arguments() = delete;
    explicit Arguments(int argc, char **argv)
        : checksum_{false}, mac_{false}, validate_{false},
          length_extension_attack_{false}, key_is_set_{false},
          mac_message_is_set_{false}, key_len_is_set_{false},
          message_extension_is_set_{false}, key_{""}, mac_message_{""},
          key_len_{0}, message_extension_{""} {
        int c;
        while ((c = getopt(argc, argv, "csvek:m:n:a:h")) != -1) {
            switch (c) {
            case 'c':
                checksum_ = true;
                break;
            case 's':
                mac_ = true;
                break;
            case 'v':
                validate_ = true;
                break;
            case 'e':
                length_extension_attack_ = true;
                break;
            case 'k':
                key_ = optarg;
                key_is_set_ = true;
                break;
            case 'm':
                mac_message_ = optarg;
                mac_message_is_set_ = true;
                break;
            case 'n':
                try {
                    key_len_ = std::stoi(optarg);
                } catch (...) {
                    std::cerr << "Error while loading key length." << std::endl;
                    exit(EXIT_FAILURE);
                }
                key_len_is_set_ = true;
                break;
            case 'a':
                message_extension_ = optarg;
                message_extension_is_set_ = true;
                break;
            case 'h':
                print_help();
                exit(EXIT_SUCCESS);
                break;
            default:
                std::cerr << "unknown option" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
    }

    /**
     * @brief Check if given arguments meet set of conditions.
     */
    bool validate_args();

    /**
     * @brief Outputs object content.
     */
    void debug();

    /**
     * @brief Print help/usage for the user.
     */
    void print_help();
};
#endif // _ARGS_HPP_
