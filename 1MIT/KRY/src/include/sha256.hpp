/**
 * Header file defining the `SHA256` class.
 *
 * This class calculates a hash value for the given input string.
 *
 * @author  Hung Do (xdohun00@stud.fit.vutbr.cz)
 * @date    29.03.2023
 * @file    sha256.hpp
 */
#ifndef _SHA256_HPP_
#define _SHA256_HPP_
#include <string.h> // memset
#include <string>   // std::string

#define HV_ARR_SIZE 8
// The size of the message block in bits
#define MESSAGE_BLOCK 512 // bits
// The size of the message block in bytes
#define MESSAGE_BLOCK_BYTES (MESSAGE_BLOCK / 8) // bytes
// Last couple of bytes for storing message size in bits
#define MESSAGE_LEN_BLOCK_BYTES 8

static u_int32_t init_hash_values[HV_ARR_SIZE] = {
    0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
    0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};

class SHA256 {
  protected:
    /** The original message */
    const char *message_;
    /** Number of message bytes that has been already processed **/
    size_t m_bytes_;
    /** The original message size */
    size_t message_len_;

    /** An array of Hash Values */
    u_int32_t h_[HV_ARR_SIZE] = {
        0,
    };
    /** Buffer containing a message block */
    char m_block_buffer_[MESSAGE_BLOCK_BYTES + 1];

  public:
    SHA256() = delete;

    /**
     * @brief SHA256 class constructor.
     *
     * @param message Input text to hash.
     */
    explicit SHA256(const std::string &message)
        : message_(message.c_str()), m_bytes_(0), message_len_(message.size()) {
        // initialize hash values
        for (int i = 0; i < HV_ARR_SIZE; i++)
            h_[i] = init_hash_values[i];
        memset(m_block_buffer_, 0, MESSAGE_BLOCK_BYTES + 1);
    }

    /**
     * @brief Pops the first block of data.
     *
     * @param offset A size of the "hidden" key.
     * @returns Number of bytes popped.
     */
    size_t get_first_block(size_t offset);

    /**
     * @brief Pop a block of data from the message.
     *
     * @returns Number of bytes popped.
     */
    size_t pop_mblock();

    /**
     * @brief Compute and update hash values for the given message block.
     */
    void compute_hash();

    /**
     * @brief Convert hash values to hex values.
     *
     * The finalized string is then appened to the given buffer.
     *
     * @param buffer An output buffer.
     */
    void digest(std::string &buffer);

    /**
     * @brief Calculates a sha256sum.
     *
     * @param out An output buffer.
     * @returns Total number of bytes used by message schedule; size of padded
     * input.
     */
    int sha256(std::string &out);

    /**
     * @brief Set initial hash values using a string in hash format.
     *
     * @param hash Hash in string value.
     */
    void set_hash_unsafe(const std::string &hash);

    /**
     * @brief Append a message and update the hash.
     *
     * @param input     A message to append.
     * @param hash      The initial hash value.
     * @param out_hash  Out-going buffer.
     */
    void sha256_extend_unsafe(const std::string &input, const std::string &hash,
                              std::string &out_hash);

    /**
     * @brief Prints out the new message
     *
     * This function generates a valid message where it appends a new text to
     * the existing message.
     *
     * @param offset        The size of the key.
     * @param append_text   New text to append.
     */
    void print_mblocks_unsafe(size_t offset, const std::string &append_text);
};

#endif // _SHA256_HPP_
