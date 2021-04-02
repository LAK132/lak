/*
 * tinflate.c -- tiny inflate library
 *
 * Written by Andrew Church <achurch@achurch.org>
 *
 * Re-written for C++ by LAK132 <https://github.com/LAK132>
 *
 * This source code is public domain.
 */

#include "lak/tinflate.hpp"

namespace tinf
{
  static constexpr uint32_t crc32_table[256] = {
    0x00000000UL, 0x77073096UL, 0xEE0E612CUL, 0x990951BAUL, 0x076DC419UL,
    0x706AF48FUL, 0xE963A535UL, 0x9E6495A3UL, 0x0EDB8832UL, 0x79DCB8A4UL,
    0xE0D5E91EUL, 0x97D2D988UL, 0x09B64C2BUL, 0x7EB17CBDUL, 0xE7B82D07UL,
    0x90BF1D91UL, 0x1DB71064UL, 0x6AB020F2UL, 0xF3B97148UL, 0x84BE41DEUL,
    0x1ADAD47DUL, 0x6DDDE4EBUL, 0xF4D4B551UL, 0x83D385C7UL, 0x136C9856UL,
    0x646BA8C0UL, 0xFD62F97AUL, 0x8A65C9ECUL, 0x14015C4FUL, 0x63066CD9UL,
    0xFA0F3D63UL, 0x8D080DF5UL, 0x3B6E20C8UL, 0x4C69105EUL, 0xD56041E4UL,
    0xA2677172UL, 0x3C03E4D1UL, 0x4B04D447UL, 0xD20D85FDUL, 0xA50AB56BUL,
    0x35B5A8FAUL, 0x42B2986CUL, 0xDBBBC9D6UL, 0xACBCF940UL, 0x32D86CE3UL,
    0x45DF5C75UL, 0xDCD60DCFUL, 0xABD13D59UL, 0x26D930ACUL, 0x51DE003AUL,
    0xC8D75180UL, 0xBFD06116UL, 0x21B4F4B5UL, 0x56B3C423UL, 0xCFBA9599UL,
    0xB8BDA50FUL, 0x2802B89EUL, 0x5F058808UL, 0xC60CD9B2UL, 0xB10BE924UL,
    0x2F6F7C87UL, 0x58684C11UL, 0xC1611DABUL, 0xB6662D3DUL, 0x76DC4190UL,
    0x01DB7106UL, 0x98D220BCUL, 0xEFD5102AUL, 0x71B18589UL, 0x06B6B51FUL,
    0x9FBFE4A5UL, 0xE8B8D433UL, 0x7807C9A2UL, 0x0F00F934UL, 0x9609A88EUL,
    0xE10E9818UL, 0x7F6A0DBBUL, 0x086D3D2DUL, 0x91646C97UL, 0xE6635C01UL,
    0x6B6B51F4UL, 0x1C6C6162UL, 0x856530D8UL, 0xF262004EUL, 0x6C0695EDUL,
    0x1B01A57BUL, 0x8208F4C1UL, 0xF50FC457UL, 0x65B0D9C6UL, 0x12B7E950UL,
    0x8BBEB8EAUL, 0xFCB9887CUL, 0x62DD1DDFUL, 0x15DA2D49UL, 0x8CD37CF3UL,
    0xFBD44C65UL, 0x4DB26158UL, 0x3AB551CEUL, 0xA3BC0074UL, 0xD4BB30E2UL,
    0x4ADFA541UL, 0x3DD895D7UL, 0xA4D1C46DUL, 0xD3D6F4FBUL, 0x4369E96AUL,
    0x346ED9FCUL, 0xAD678846UL, 0xDA60B8D0UL, 0x44042D73UL, 0x33031DE5UL,
    0xAA0A4C5FUL, 0xDD0D7CC9UL, 0x5005713CUL, 0x270241AAUL, 0xBE0B1010UL,
    0xC90C2086UL, 0x5768B525UL, 0x206F85B3UL, 0xB966D409UL, 0xCE61E49FUL,
    0x5EDEF90EUL, 0x29D9C998UL, 0xB0D09822UL, 0xC7D7A8B4UL, 0x59B33D17UL,
    0x2EB40D81UL, 0xB7BD5C3BUL, 0xC0BA6CADUL, 0xEDB88320UL, 0x9ABFB3B6UL,
    0x03B6E20CUL, 0x74B1D29AUL, 0xEAD54739UL, 0x9DD277AFUL, 0x04DB2615UL,
    0x73DC1683UL, 0xE3630B12UL, 0x94643B84UL, 0x0D6D6A3EUL, 0x7A6A5AA8UL,
    0xE40ECF0BUL, 0x9309FF9DUL, 0x0A00AE27UL, 0x7D079EB1UL, 0xF00F9344UL,
    0x8708A3D2UL, 0x1E01F268UL, 0x6906C2FEUL, 0xF762575DUL, 0x806567CBUL,
    0x196C3671UL, 0x6E6B06E7UL, 0xFED41B76UL, 0x89D32BE0UL, 0x10DA7A5AUL,
    0x67DD4ACCUL, 0xF9B9DF6FUL, 0x8EBEEFF9UL, 0x17B7BE43UL, 0x60B08ED5UL,
    0xD6D6A3E8UL, 0xA1D1937EUL, 0x38D8C2C4UL, 0x4FDFF252UL, 0xD1BB67F1UL,
    0xA6BC5767UL, 0x3FB506DDUL, 0x48B2364BUL, 0xD80D2BDAUL, 0xAF0A1B4CUL,
    0x36034AF6UL, 0x41047A60UL, 0xDF60EFC3UL, 0xA867DF55UL, 0x316E8EEFUL,
    0x4669BE79UL, 0xCB61B38CUL, 0xBC66831AUL, 0x256FD2A0UL, 0x5268E236UL,
    0xCC0C7795UL, 0xBB0B4703UL, 0x220216B9UL, 0x5505262FUL, 0xC5BA3BBEUL,
    0xB2BD0B28UL, 0x2BB45A92UL, 0x5CB36A04UL, 0xC2D7FFA7UL, 0xB5D0CF31UL,
    0x2CD99E8BUL, 0x5BDEAE1DUL, 0x9B64C2B0UL, 0xEC63F226UL, 0x756AA39CUL,
    0x026D930AUL, 0x9C0906A9UL, 0xEB0E363FUL, 0x72076785UL, 0x05005713UL,
    0x95BF4A82UL, 0xE2B87A14UL, 0x7BB12BAEUL, 0x0CB61B38UL, 0x92D28E9BUL,
    0xE5D5BE0DUL, 0x7CDCEFB7UL, 0x0BDBDF21UL, 0x86D3D2D4UL, 0xF1D4E242UL,
    0x68DDB3F8UL, 0x1FDA836EUL, 0x81BE16CDUL, 0xF6B9265BUL, 0x6FB077E1UL,
    0x18B74777UL, 0x88085AE6UL, 0xFF0F6A70UL, 0x66063BCAUL, 0x11010B5CUL,
    0x8F659EFFUL, 0xF862AE69UL, 0x616BFFD3UL, 0x166CCF45UL, 0xA00AE278UL,
    0xD70DD2EEUL, 0x4E048354UL, 0x3903B3C2UL, 0xA7672661UL, 0xD06016F7UL,
    0x4969474DUL, 0x3E6E77DBUL, 0xAED16A4AUL, 0xD9D65ADCUL, 0x40DF0B66UL,
    0x37D83BF0UL, 0xA9BCAE53UL, 0xDEBB9EC5UL, 0x47B2CF7FUL, 0x30B5FFE9UL,
    0xBDBDF21CUL, 0xCABAC28AUL, 0x53B39330UL, 0x24B4A3A6UL, 0xBAD03605UL,
    0xCDD70693UL, 0x54DE5729UL, 0x23D967BFUL, 0xB3667A2EUL, 0xC4614AB8UL,
    0x5D681B02UL, 0x2A6F2B94UL, 0xB40BBE37UL, 0xC30C8EA1UL, 0x5A05DF1BUL,
    0x2D02EF8DUL};

  const char *error_name(error_t error)
  {
    switch (error)
    {
      case error_t::OK: return "Ok";
      case error_t::NO_DATA: return "No data";
      case error_t::INVALID_PARAMETER: return "Invalid parameter";
      case error_t::CUSTOM_DICTIONARY: return "Custom dictionary";
      case error_t::INVALID_STATE: return "Invalid state";
      case error_t::INVALID_BLOCK_CODE: return "Invalid block code";
      case error_t::OUT_OF_DATA: return "Out of data";
      case error_t::OUTPUT_FULL: return "Output buffer full";
      case error_t::CORRUPT_STREAM: return "Corrupt stream";
      case error_t::HUFFMAN_TABLE_GEN_FAILED:
        return "Huffman table gen failed";
      case error_t::INVALID_SYMBOL: return "Invalid symbol";
      case error_t::INVALID_DISTANCE: return "Invalid distance";
      case error_t::NO_SYMBOLS: return "No symbols";
      case error_t::TOO_MANY_SYMBOLS: return "Too many symbols";
      case error_t::INCOMPLETE_TREE: return "Incomplete tree";
      default: return "Not a tinflate error";
    }
  }

  error_t tinflate(lak::span<const uint8_t> compressed,
                   lak::array<uint8_t> *output,
                   uint32_t *crc)
  {
    decompression_state_t state;
    return tinflate(compressed, output, state, crc);
  }

  error_t tinflate(lak::span<const uint8_t> compressed,
                   lak::array<uint8_t> *output,
                   decompression_state_t &state,
                   uint32_t *crc)
  {
    auto &buffer = *output;
    if (buffer.empty()) buffer.resize(0x100);

    auto *head = buffer.data();

    error_t err = error_t::OK;
    while (err == error_t::OK && !state.final)
    {
      err = tinflate(compressed, lak::span(buffer), &head, state, crc);
      while (err == tinf::error_t::OUTPUT_FULL)
      {
        size_t offset = head - buffer.data();
        buffer.resize(buffer.size() * 2);
        head = buffer.data() + offset;
        err  = tinflate(compressed, lak::span(buffer), &head, state, crc);
      }
    }

    return err;
  }

  error_t tinflate(lak::span<const uint8_t> compressed,
                   lak::span<uint8_t> output,
                   uint8_t **head,
                   uint32_t *crc)
  {
    decompression_state_t state;
    return tinflate(compressed, output, head, state, crc);
  }

  error_t tinflate(lak::span<const uint8_t> compressed,
                   lak::span<uint8_t> output,
                   uint8_t **head,
                   decompression_state_t &state,
                   uint32_t *crc)
  {
    state.data = compressed;

    if (error_t er = tinflate_header(state); er != error_t::OK) return er;

    do
    {
      error_t res = tinflate_block(output, head, state);
      if (res != error_t::OK) return res;
    } while (!state.final);

    if (crc) *crc = state.crc;

    return error_t::OK;
  }

  error_t tinflate_header(decompression_state_t &state)
  {
    if (state.data.empty()) return error_t::OUT_OF_DATA;

    if (state.state == state_t::INITIAL ||
        state.state == state_t::PARTIAL_ZLIB_HEADER)
    {
      uint16_t zlibHeader;
      if (state.data.size() == 0)
      {
        return error_t::OUT_OF_DATA;
      }

      if (state.state == state_t::INITIAL && state.data.size() == 1)
      {
        state.first_byte = state.data[0];
        state.state      = state_t::PARTIAL_ZLIB_HEADER;
        return error_t::OUT_OF_DATA;
      }

      if (state.state == state_t::PARTIAL_ZLIB_HEADER)
      {
        zlibHeader = (state.first_byte << 8) | state.data[0];
      }
      else
      {
        zlibHeader = (state.data[0] << 8) | state.data[1];
      }

      if ((zlibHeader & 0x8F00) == 0x0800 && (zlibHeader % 31) == 0)
      {
        if (zlibHeader & 0x0020) return error_t::CUSTOM_DICTIONARY;

        const unsigned int increment =
          (state.state == state_t::PARTIAL_ZLIB_HEADER ? 1 : 2);
        if (increment > state.data.size()) return error_t::OUT_OF_DATA;
        state.data = state.data.subspan(increment);
      }
      else if (state.state == state_t::PARTIAL_ZLIB_HEADER)
      {
        state.bit_accum = state.first_byte;
        state.num_bits  = 8;
      }
      state.state = state_t::HEADER;
    }

    return error_t::OK;
  }

  error_t tinflate_block(lak::span<uint8_t> output,
                         uint8_t **head,
                         decompression_state_t &state)
  {
    uint32_t icrc = ~state.crc;
    if (*head < output.begin()) *head = output.begin();
    if (*head >= output.end()) return error_t::OUTPUT_FULL;

    auto push = [&](const uint8_t val) -> bool {
      if (*head >= output.end()) return false;
      *((*head)++) = val;
      icrc = crc32_table[(icrc & 0xFF) ^ val] ^ ((icrc >> 8) & 0xFF'FFFFUL);
      return true;
    };

    auto out_of_data = [&state, &icrc]() -> error_t {
      state.crc = ~icrc & 0xFFFF'FFFFUL;
      return error_t::OUT_OF_DATA;
    };

    static const uint8_t codelen_order[19] = {
      16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
    static const uint8_t codelen_order_anaconda[19] = {
      18, 17, 16, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

#define SET_STATE(STATE)                                                      \
  state.state = state_t::STATE;                                               \
  [[fallthrough]];                                                            \
  case state_t::STATE:

    switch (state.state)
    {
      case state_t::HEADER:
        if (state.anaconda)
        {
          if (state.get_bits(4, &state.block_type)) return out_of_data();
          state.final = state.block_type >> 3;
          state.block_type &= 0x7;
          if (state.block_type == 7)
            state.block_type = 0;
          else if (state.block_type == 5)
            state.block_type = 1;
          else if (state.block_type == 6)
            state.block_type = 2;
          else
            state.block_type = 3;
        }
        else
        {
          if (state.get_bits(3, &state.block_type)) return out_of_data();
          state.final = state.block_type & 0x1;
          state.block_type >>= 1;
        }

        if (state.block_type == 3)
        {
          state.crc = ~icrc & 0xFFFF'FFFFUL;
          return error_t::INVALID_BLOCK_CODE;
        }

        if (state.block_type == 0)
        {
          state.num_bits = 0;

          SET_STATE(UNCOMPRESSED_LEN)

          if (state.get_bits(16, &state.len)) return out_of_data();

          SET_STATE(UNCOMPRESSED_ILEN)

          if (!state.anaconda)
          {
            if (state.get_bits(16, &state.ilen)) return out_of_data();

            if (state.ilen != (~state.len & 0xFFFF))
            {
              state.crc = ~icrc & 0xFFFF'FFFFUL;
              return error_t::CORRUPT_STREAM;
            }
          }
          state.nread = 0;

          SET_STATE(UNCOMPRESSED_DATA)

          while (state.nread < state.len)
          {
            if (state.data.empty()) return out_of_data();

            if (!push(state.data[0])) return error_t::OUTPUT_FULL;

            state.data = state.data.subspan(1);
            ++state.nread;
          }
          state.crc   = ~icrc & 0xFFFF'FFFFUL;
          state.state = state_t::HEADER;
          return error_t::OK;
        }

        if (state.block_type == 2)
        {
          SET_STATE(LITERAL_COUNT)

          if (state.get_bits(5, &state.literal_count)) return out_of_data();
          state.literal_count += 257;

          SET_STATE(DISTANCE_COUNT)

          if (state.get_bits(5, &state.distance_count)) return out_of_data();
          state.distance_count += 1;

          SET_STATE(CODELEN_COUNT)

          if (state.get_bits(4, &state.codelen_count)) return out_of_data();
          state.codelen_count += 4;
          state.counter = 0;

          SET_STATE(READ_CODE_LENGTHS)

          if (state.anaconda)
          {
            for (; state.counter < state.codelen_count; ++state.counter)
              if (state.get_bits(
                    3,
                    &state.codelen_len[codelen_order_anaconda[state.counter]]))
                return out_of_data();

            for (; state.counter < 19; ++state.counter)
              state.codelen_len[codelen_order_anaconda[state.counter]] = 0;
          }
          else
          {
            for (; state.counter < state.codelen_count; ++state.counter)
              if (state.get_bits(
                    3, &state.codelen_len[codelen_order[state.counter]]))
                return out_of_data();

            for (; state.counter < 19; ++state.counter)
              state.codelen_len[codelen_order[state.counter]] = 0;
          }

          if (gen_huffman_table(
                19, state.codelen_len, false, state.codelen_table) !=
              error_t::OK)
          {
            state.crc = ~icrc & 0xFFFF'FFFFUL;
            return error_t::HUFFMAN_TABLE_GEN_FAILED;
          }

          state.repeat_count = 0;
          state.last_value   = 0;
          state.counter      = 0;

          SET_STATE(READ_LENGTHS)

          while (state.counter < state.literal_count + state.distance_count)
          {
            if (state.repeat_count == 0)
            {
              if (state.get_huff(state.codelen_table, &state.symbol))
                return out_of_data();

              if (state.symbol < 16)
              {
                state.last_value   = state.symbol;
                state.repeat_count = 1;
              }
              else if (state.symbol == 16)
              {
                SET_STATE(READ_LENGTHS_16)

                if (state.get_bits(2, &state.repeat_count))
                  return out_of_data();
                state.repeat_count += 3;
              }
              else if (state.symbol == 17)
              {
                state.last_value = 0;

                SET_STATE(READ_LENGTHS_17)

                if (state.get_bits(3, &state.repeat_count))
                  return out_of_data();
                state.repeat_count += 3;
              }
              else if (state.symbol)
              {
                state.last_value = 0;

                SET_STATE(READ_LENGTHS_18)

                if (state.get_bits(7, &state.repeat_count))
                  return out_of_data();
                state.repeat_count += 11;
              }
            }

            if (state.counter < state.literal_count)
              state.literal_len[state.counter] = (uint8_t)state.last_value;
            else
              state.distance_len[state.counter - state.literal_count] =
                (uint8_t)state.last_value;

            ++state.counter;
            --state.repeat_count;
            state.state = state_t::READ_LENGTHS;
          }

          if (gen_huffman_table(state.literal_count,
                                state.literal_len,
                                false,
                                state.literal_table) != error_t::OK ||
              gen_huffman_table(state.distance_count,
                                state.distance_len,
                                true,
                                state.distance_table) != error_t::OK)
          {
            state.crc = ~icrc & 0xFFFF'FFFFUL;
            return error_t::HUFFMAN_TABLE_GEN_FAILED;
          }
        }
        else
        {
          int32_t next_free = 2;
          int32_t i;

          for (i = 0; i < 0x7E; ++i)
          {
            state.literal_table[i] = ~next_free;
            next_free += 2;
          }

          for (; i < 0x96; ++i)
            state.literal_table[i] = (uint16_t)i + (256 - 0x7E);

          for (; i < 0xFE; ++i)
          {
            state.literal_table[i] = ~next_free;
            next_free += 2;
          }

          for (; i < 0x18E; ++i)
            state.literal_table[i] = (uint16_t)i + (0 - 0xFE);

          for (; i < 0x196; ++i)
            state.literal_table[i] = (uint16_t)i + (280 - 0x18E);

          for (; i < 0x1CE; ++i)
          {
            state.literal_table[i] = ~next_free;
            next_free += 2;
          }

          for (; i < 0x23E; ++i)
            state.literal_table[i] = (uint16_t)i + (144 - 0x1CE);

          for (i = 0; i < 0x1E; ++i) state.distance_table[i] = ~(i * 2 + 2);

          for (i = 0x1E; i < 0x3E; ++i) state.distance_table[i] = i - 0x1E;
        }

        for (;;)
        {
          SET_STATE(READ_SYMBOL)

          if (state.get_huff(state.literal_table, &state.symbol))
            return out_of_data();

          if (state.symbol < 256)
          {
            SET_STATE(PUSH_SYMBOL)

            if (!push((uint8_t)state.symbol)) return error_t::OUTPUT_FULL;

            continue;
          }

          if (state.symbol == 256) break;

          if (state.symbol <= 264)
          {
            state.repeat_length = (state.symbol - 257) + 3;
          }
          else if (state.symbol <= 284)
          {
            SET_STATE(READ_LENGTH)

            const uint32_t lengthBits = (state.symbol - 261) / 4;
            if (state.get_bits(lengthBits, &state.repeat_length))
              return out_of_data();
            state.repeat_length +=
              3 + ((4 + ((state.symbol - 265) & 3)) << lengthBits);
          }
          else if (state.symbol == 285)
          {
            state.repeat_length = 258;
          }
          else
          {
            state.crc = ~icrc & 0xFFFF'FFFFUL;
            return error_t::INVALID_SYMBOL;
          }

          SET_STATE(READ_DISTANCE)

          if (state.get_huff(state.distance_table, &state.symbol))
            return out_of_data();

          if (state.symbol <= 3)
          {
            state.distance = state.symbol + 1;
          }
          else if (state.symbol <= 29)
          {
            SET_STATE(READ_DISTANCE_EXTRA)

            const uint32_t distance_bits = (state.symbol - 2) / 2;
            if (state.get_bits(distance_bits, &state.distance))
              return out_of_data();
            state.distance += 1 + ((2 + (state.symbol & 1)) << distance_bits);
          }
          else
          {
            state.crc = ~icrc & 0xFFFF'FFFFUL;
            return error_t::INVALID_SYMBOL;
          }

          if (state.distance > *head - output.begin())
          {
            state.crc = ~icrc & 0xFFFF'FFFFUL;
            return error_t::INVALID_DISTANCE;
            // There wasn't enough data in the back buffer.
          }

          SET_STATE(WRITE_REPEAT)

          for (; state.repeat_length > 0; --state.repeat_length)
            if (!push(*((*head) - state.distance)))
              return error_t::OUTPUT_FULL;
          state.repeat_length = 0;
        }
        break;

      case state_t::INITIAL: [[fallthrough]];
      case state_t::PARTIAL_ZLIB_HEADER: [[fallthrough]];
      default: return error_t::INVALID_STATE;
    }
#undef SET_STATE

    state.crc   = ~icrc & 0xFFFF'FFFFUL;
    state.state = state_t::HEADER;
    return error_t::OK;
  }

  error_t gen_huffman_table(uint32_t symbols,
                            const uint8_t *lengths,
                            bool allow_no_symbols,
                            int16_t *table)
  {
    uint16_t length_count[16] = {0};
    uint16_t total_count      = 0;
    uint16_t first_code[16];

    for (uint32_t i = 0; i < symbols; ++i)
      if (lengths[i] > 0) ++length_count[lengths[i]];

    for (uint32_t i = 1; i < 16; ++i) total_count += length_count[i];

    if (total_count == 0)
    {
      return allow_no_symbols ? error_t::OK : error_t::NO_SYMBOLS;
    }
    else if (total_count == 1)
    {
      for (uint32_t i = 0; i < symbols; ++i)
        if (lengths[i] != 0) table[0] = table[1] = (uint16_t)i;
      return error_t::OK;
    }

    first_code[0] = 0;
    for (uint32_t i = 1; i < 16; ++i)
    {
      first_code[i] = (first_code[i - 1] + length_count[i - 1]) << 1;
      if (first_code[i] + length_count[i] > (uint16_t)1 << i)
        return error_t::TOO_MANY_SYMBOLS;
    }
    if (first_code[15] + length_count[15] != (uint16_t)1 << 15)
      return error_t::INCOMPLETE_TREE;

    for (uint32_t index = 0, i = 1; i < 16; ++i)
    {
      uint32_t code_limit = 1U << i;
      uint32_t next_code  = first_code[i] + length_count[i];
      uint32_t next_index = index + (code_limit - first_code[i]);

      for (uint32_t j = 0; j < symbols; ++j)
        if (lengths[j] == i) table[index++] = (uint16_t)j;

      for (uint32_t j = next_code; j < code_limit; ++j)
      {
        table[index++] = ~next_index;
        next_index += 2;
      }
    }

    return error_t::OK;
  }
}
