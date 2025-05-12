/**
 * Implementation of `SHA256` class and sha256 algorithm.
 *
 * @author  Hung Do (xdohun00@stud.fit.vutbr.cz)
 * @date    29.03.2023
 * @file    sha256.cpp
 */

#include "sha256.hpp"
#include <assert.h> // assert
#include <bitset>   // std::bitset
#include <cctype>   // std::isprint
#include <iostream> // std::cout
#include <stdio.h>  // printf

#define _WORD_SIZE 32 // bits

// operations of words
#define ROTR(x, n) ((x >> n) | (x << (_WORD_SIZE - n)))
#define SHR(x, n) (x >> n)

// SHA256 functions
#define CH(x, y, z) ((x & y) ^ (~x & z))
#define MAJ(x, y, z) ((x & y) ^ (x & z) ^ (y & z))
#define BIG_SIGMA_0(x) (ROTR(x, 2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define BIG_SIGMA_1(x) (ROTR(x, 6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define SIGMA_0(x) (ROTR(x, 7) ^ ROTR(x, 18) ^ SHR(x, 3))
#define SIGMA_1(x) (ROTR(x, 17) ^ ROTR(x, 19) ^ SHR(x, 10))

// constants used to calculate SHA-256
static const u_int32_t K[] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,
    0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
    0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
    0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
    0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
    0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2,
};

/**
 * @brief Store a 64bit value in the buffer.
 * The buffer must have at least 8 bytes alocated.
 *
 * @param buf   An output buffer.
 * @param value A value to store in the buffer.
 * @return Number of bytes written.
 */
static int set_u64_in_buffer(char *buf, u_int64_t value) {
    for (int i = MESSAGE_LEN_BLOCK_BYTES - 1; i >= 0; value >>= 8, i--) {
        buf[i] = value & 0xff;
    }
    return MESSAGE_LEN_BLOCK_BYTES;
}

/**
 * @brief Fill a buffer with zeros.
 *
 * @param buf       An output buffer.
 * @param nof_zeros Number of bytes to set to zero.
 * @return Number of bytes written.
 */
static int set_zeros_in_buffer(char *buff, size_t nof_zeros) {
    memset(buff, 0, nof_zeros);
    return nof_zeros;
}

/**
 * @brief Prepare the message schedule before starting computing hash.
 *
 * @param m_block       A block of data.
 * @param m_block_size  A size of the block.
 * @param w             An array of words.
 * @param w_size        Size of array of words.
 */
static void prepare_message_schedule(const char *m_block, size_t m_block_size,
                                     u_int32_t *w, size_t w_size) {
    assert(m_block_size == MESSAGE_BLOCK_BYTES && w_size == 64);
    int i = 0;
    for (; i < 16; i++) {
        w[i] |= (u_int32_t)m_block[i * 4] & 0xff;
        w[i] <<= 8;
        w[i] |= (u_int32_t)m_block[i * 4 + 1] & 0xff;
        w[i] <<= 8;
        w[i] |= (u_int32_t)m_block[i * 4 + 2] & 0xff;
        w[i] <<= 8;
        w[i] |= (u_int32_t)m_block[i * 4 + 3] & 0xff;
    }

    for (; i < 64; i++) {
        // addition is performed modulo 2^32
        // it is expected for the value to overflow
        w[i] = SIGMA_1(w[i - 2]) + w[i - 7] + SIGMA_0(w[i - 15]) + w[i - 16];
    }
}

size_t SHA256::pop_mblock() {
    if (m_bytes_ > message_len_ + MESSAGE_LEN_BLOCK_BYTES)
        return 0;

    int remaining_size = message_len_ - m_bytes_;
    int offset = 0;

    // fill the last block with zeros and message size
    if (remaining_size < 0) {
        // set zeros
        memset(m_block_buffer_, 0,
               MESSAGE_BLOCK_BYTES - MESSAGE_LEN_BLOCK_BYTES);
        offset += (MESSAGE_BLOCK_BYTES - MESSAGE_LEN_BLOCK_BYTES);
        offset += set_u64_in_buffer(m_block_buffer_ + offset, message_len_ * 8);
    } else if (remaining_size >= 64) {
        memcpy(m_block_buffer_, message_, MESSAGE_BLOCK_BYTES);
        offset += MESSAGE_BLOCK_BYTES;
        message_ += offset;
    } else {
        // copy message
        memcpy(m_block_buffer_, message_, remaining_size);
        offset += remaining_size;
        message_ += offset;

        // set delimiter
        memset(m_block_buffer_ + offset, 0x80, 1);
        offset += 1;

        // set the last couple of bytes in the block
        // if there is no space left to set the message size, the function
        // will set it in the next iteration
        if (MESSAGE_BLOCK_BYTES - offset < MESSAGE_LEN_BLOCK_BYTES) {
            offset += set_zeros_in_buffer(m_block_buffer_ + offset,
                                          MESSAGE_BLOCK_BYTES - offset);
        } else {
            offset += set_zeros_in_buffer(m_block_buffer_ + offset,
                                          MESSAGE_BLOCK_BYTES - offset -
                                              MESSAGE_LEN_BLOCK_BYTES);

            // set message size
            offset +=
                set_u64_in_buffer(m_block_buffer_ + offset, message_len_ * 8);
        }
    }

    // check the size of the offset as it must equal to the size of the block
    m_bytes_ += offset;
    assert(offset == MESSAGE_BLOCK_BYTES);

    return offset;
}

void SHA256::compute_hash() {
    // words of message schedule
    u_int32_t w[64] = {
        0,
    };
    prepare_message_schedule(m_block_buffer_, MESSAGE_BLOCK_BYTES, w, 64);

    // working variables
    u_int32_t a = h_[0];
    u_int32_t b = h_[1];
    u_int32_t c = h_[2];
    u_int32_t d = h_[3];
    u_int32_t e = h_[4];
    u_int32_t f = h_[5];
    u_int32_t g = h_[6];
    u_int32_t h = h_[7];
    u_int32_t t1, t2;
    // ----------------------------

    // value updates
    for (int i = 0; i < 64; i++) {
        t1 = h + BIG_SIGMA_1(e) + CH(e, f, g) + K[i] + w[i];
        t2 = BIG_SIGMA_0(a) + MAJ(a, b, c);
        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }

    // ----------------------------
    h_[0] = a + h_[0];
    h_[1] = b + h_[1];
    h_[2] = c + h_[2];
    h_[3] = d + h_[3];
    h_[4] = e + h_[4];
    h_[5] = f + h_[5];
    h_[6] = g + h_[6];
    h_[7] = h + h_[7];
}

void SHA256::digest(std::string &buffer) {
    char tmp[9] = {
        0,
    };
    memset(tmp, 0, 9);
    // iterate through all hash values
    for (int i = 0; i < 8; i++) {
        u_int32_t h_val = h_[i];

        // convert to hex value
        for (int j = 7; j >= 0; j--, h_val >>= 4) {
            char hex = h_val & 0x0f;
            tmp[j] = hex < 10 ? '0' + hex : 'a' + (hex - 10);
        }
        buffer.append(tmp);
    }
}

int SHA256::sha256(std::string &out) {
    out = "";
    while (pop_mblock()) {
        compute_hash();
    }
    digest(out);
    return m_bytes_;
}

void SHA256::set_hash_unsafe(const std::string &hash) {
    assert(hash.size() == HV_ARR_SIZE * 8);
    const char *ptr = hash.c_str();

    u_char hex_val = 0;
    u_int32_t h_val = 0;

    // iterate through h_
    for (size_t i = 0; i < HV_ARR_SIZE; i++) {
        // construct new h_ value from string
        for (size_t j = 0; j < 8; j++, ptr++) {
            if (*ptr <= '9') {
                hex_val = *ptr - '0';
            } else {
                hex_val = *ptr - 'a' + 10;
            }
            h_val <<= 4;
            h_val &= ~0x0f;
            h_val |= (hex_val & 0x0f);
        }
        h_[i] = h_val;
    }
}

void SHA256::sha256_extend_unsafe(const std::string &input,
                                  const std::string &hash,
                                  std::string &out_hash) {
    // calculate new hash
    set_hash_unsafe(hash);
    message_ = input.c_str();
    message_len_ = MESSAGE_BLOCK_BYTES + input.size();
    m_bytes_ = MESSAGE_BLOCK_BYTES;

    sha256(out_hash);
}

size_t SHA256::get_first_block(size_t offset) {
    // calculate the total size of the message with the key
    size_t msg_total_len = offset + (message_len_ - m_bytes_);

    // if the total message size is greater than the block
    // just pop the block
    if (msg_total_len >= 64) {
        // calculate the "known" part of the message
        size_t block_msg_len = MESSAGE_BLOCK_BYTES - offset;
        memcpy(m_block_buffer_, message_, block_msg_len);

        m_bytes_ += block_msg_len;
        message_ += block_msg_len;
        return block_msg_len;
    }

    // if the total message length is less than the block
    // that means that we will have to generate the padding
    // first, we copy the "known" message to the working buffer
    int curr_offset = message_len_ - m_bytes_;
    memcpy(m_block_buffer_, message_, curr_offset);
    message_ += curr_offset;

    // a special case where the size of the message and the key is
    // exactly the size of the block
    if (offset + curr_offset == 64) {
        return curr_offset;
    }

    // append the boundary bit
    memset(m_block_buffer_ + curr_offset, 0x80, 1);
    curr_offset += 1;

    // fill with zeros if there is not enough space for the message length
    if (MESSAGE_BLOCK_BYTES - curr_offset - offset < MESSAGE_LEN_BLOCK_BYTES) {
        curr_offset +=
            set_zeros_in_buffer(m_block_buffer_ + curr_offset,
                                MESSAGE_BLOCK_BYTES - curr_offset - offset);
    } else {
        // else finish the padding
        curr_offset +=
            set_zeros_in_buffer(m_block_buffer_ + curr_offset,
                                MESSAGE_BLOCK_BYTES - curr_offset - offset -
                                    MESSAGE_LEN_BLOCK_BYTES);

        // set message size
        curr_offset +=
            set_u64_in_buffer(m_block_buffer_ + curr_offset, message_len_ * 8);
    }
    m_bytes_ += curr_offset;

    return curr_offset;
}

void SHA256::print_mblocks_unsafe(size_t offset,
                                  const std::string &append_text) {
    message_len_ += offset;
    m_bytes_ = offset;
    size_t i = 0;
    bool is_padding = false;

    // apply the key offset to the first message block
    size_t size = get_first_block(offset % 64);
    do {
        // print the message block buffer
        for (i = 0; i < size; i++) {
            int c = ((int)m_block_buffer_[i]) & 0xff;

            // choose a format to print: char or hex
            if (std::isprint(c) && !is_padding) {
                std::cout << (char)c;
            } else {
                printf("\\x%02x", c);
                is_padding = true;
            }
        }

    } while ((size = pop_mblock()));

    // append the final text
    std::cout << append_text << std::endl;
}
/* sha256.cpp */
