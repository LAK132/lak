/*
 * tinflate.c -- tiny inflate library
 *
 * Written by Andrew Church <achurch@achurch.org>
 *
 * Re-written for C++ by LAK132 <https://github.com/LAK132>
 *
 * This source code is public domain.
 */

#ifndef LAK_TINFLATE_HPP
#define LAK_TINFLATE_HPP

#include "lak/array.hpp"
#include "lak/span.hpp"
#include "lak/stdint.hpp"

namespace tinf
{
  enum class state_t : uint8_t
  {
    INITIAL = 0,
    PARTIAL_ZLIB_HEADER,
    HEADER,
    UNCOMPRESSED_LEN,
    UNCOMPRESSED_ILEN,
    UNCOMPRESSED_DATA,
    LITERAL_COUNT,
    DISTANCE_COUNT,
    CODELEN_COUNT,
    READ_CODE_LENGTHS,
    READ_LENGTHS,
    READ_LENGTHS_16,
    READ_LENGTHS_17,
    READ_LENGTHS_18,
    READ_SYMBOL,
    PUSH_SYMBOL,
    READ_LENGTH,
    READ_DISTANCE,
    READ_DISTANCE_EXTRA,
    WRITE_REPEAT
  };

// Fuck you Windows
#undef NO_DATA

  enum class error_t : uint8_t
  {
    OK,

    NO_DATA,

    INVALID_PARAMETER,
    CUSTOM_DICTIONARY,

    INVALID_STATE,
    INVALID_BLOCK_CODE,
    OUT_OF_DATA,
    OUTPUT_FULL, // output buffer needs more memory. output buffer must contain
                 // at least the last 32KiB of read data.
    CORRUPT_STREAM,
    HUFFMAN_TABLE_GEN_FAILED,
    INVALID_SYMBOL,
    INVALID_DISTANCE,

    NO_SYMBOLS,
    TOO_MANY_SYMBOLS,
    INCOMPLETE_TREE
  };

  struct decompression_state_t
  {
    lak::span<const uint8_t> data;

    state_t state      = state_t::INITIAL;
    uint32_t crc       = 0;
    uint32_t bit_accum = 0;
    uint32_t num_bits  = 0;
    bool final         = false;
    bool anaconda      = false;

    uint8_t first_byte;
    uint8_t block_type;
    uint32_t counter;
    uint32_t symbol;
    uint32_t last_value;
    uint32_t repeat_length;
    uint32_t repeat_count;
    uint32_t distance;

    uint32_t len;
    uint32_t ilen;
    uint32_t nread;

    int16_t literal_table[0x23E]; // (288 * 2) - 2
    int16_t distance_table[0x3E]; // (32 * 2) - 2
    uint32_t literal_count;
    uint32_t distance_count;
    uint32_t codelen_count;
    int16_t codelen_table[0x24]; // (19 * 2) - 2
    uint8_t literal_len[0x120];  // 288
    uint8_t distance_len[0x20];  // 32
    uint8_t codelen_len[0x13];   // 19

    template<typename T>
    bool peek_bits(const size_t n, T &var)
    {
      while (num_bits < n)
      {
        if (data.empty()) return true;
        bit_accum |= data[0] << num_bits;
        num_bits += 8;
        data = data.subspan(1);
      }
      var = bit_accum & ((1UL << n) - 1);
      return false;
    }

    force_inline bool get_byte()
    {
      ASSERT_LESS_OR_EQUAL(num_bits + 8, sizeof(bit_accum) * 8);
      if (data.empty()) return true;
      bit_accum |= data[0] << num_bits;
      num_bits += 8;
      data = data.subspan(1);
      return false;
    }

    force_inline void flush_bits(const size_t n)
    {
      bit_accum >>= n;
      num_bits -= n;
    }

    template<typename T>
    bool get_bits(const size_t n, T *out)
    {
      while (num_bits < n)
        if (get_byte()) return true;

      *out = bit_accum & ((1UL << n) - 1);

      flush_bits(n);

      return false;
    }

    template<typename T>
    bool get_huff(int16_t *table, T *out)
    {
      uint32_t bitsUsed = 0;
      uint32_t index    = 0;

      for (;;)
      {
        if (num_bits <= bitsUsed)
          if (get_byte()) return true;

        index += (bit_accum >> bitsUsed) & 1;
        ++bitsUsed;
        if (table[index] >= 0) break;
        index = ~(table[index]);
      }

      *out = table[index];

      flush_bits(bitsUsed);

      return false;
    }
  };

  const char *error_name(error_t error);

  // the output buffer should always contain the last 32KiB data read.
  //
  // if *head is null it will be set to output.begin().

  error_t tinflate(lak::span<const uint8_t> compressed,
                   lak::array<uint8_t> *output,
                   uint32_t *crc = nullptr);

  error_t tinflate(lak::span<const uint8_t> compressed,
                   lak::array<uint8_t> *output,
                   decompression_state_t &state,
                   uint32_t *crc = nullptr);

  error_t tinflate(lak::span<const uint8_t> compressed,
                   lak::span<uint8_t> output,
                   uint8_t **head,
                   uint32_t *crc = nullptr);

  error_t tinflate(lak::span<const uint8_t> compressed,
                   lak::span<uint8_t> output,
                   uint8_t **head,
                   decompression_state_t &state,
                   uint32_t *crc = nullptr);

  error_t tinflate_header(decompression_state_t &state);

  error_t tinflate_block(lak::span<uint8_t> output,
                         uint8_t **head,
                         decompression_state_t &state);

  error_t gen_huffman_table(uint32_t symbols,
                            const uint8_t *lengths,
                            bool allow_no_symbols,
                            int16_t *table);
}

#endif // LAK_TINFLATE_HPP
