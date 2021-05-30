#include "lak/compression/deflate.hpp"

namespace lak
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
}

inline lak::deflate_iterator &lak::deflate_iterator::fail(error_t reason)
{
  ASSERT(reason != error_t::ok);
  if (reason != error_t::out_of_data) DEBUG_BREAK();
  _crc   = ~_icrc & 0xFFFF'FFFFUL;
  _value = value_type::make_err(reason);
  return *this;
}

inline lak::deflate_iterator &lak::deflate_iterator::success(uint8_t value)
{
  _output_buffer.push_back(value);
  _value = value_type::make_ok(value);
  _icrc  = crc32_table[(_icrc & 0xFF) ^ value] ^ ((_icrc >> 8) & 0xFF'FFFFUL);
  _crc   = ~_icrc & 0xFFFF'FFFFUL;
  return *this;
}

inline lak::deflate_iterator &lak::deflate_iterator::block_complete()
{
  _state = state_t::header;
  _crc   = ~_icrc & 0xFFFF'FFFFUL;
  _value = value_type::make_err(error_t::ok);
  return *this;
}

inline lak::deflate_iterator &lak::deflate_iterator::step()
{
  _icrc = ~_crc;

  static constexpr uint8_t codelen_order[19] = {
    16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
  static constexpr uint8_t codelen_order_anaconda[19] = {
    18, 17, 16, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

#define SET_STATE(STATE)                                                      \
  _state = state_t::STATE;                                                    \
  [[fallthrough]];                                                            \
  case state_t::STATE:

  switch (_state)
  {
    case state_t::out_of_data:
    {
      _value = value_type::make_err(error_t::out_of_data);
      return *this;
    }

    case state_t::custom_dictionary:
    {
      _value = value_type::make_err(error_t::custom_dictionary);
      return *this;
    }

    case state_t::header:
      if (_anaconda)
      {
        if (_compressed.read_bits(4)
              .if_ok([&](uintmax_t v) { _block_type = v; })
              .is_err())
          return fail(error_t::out_of_data);

        _final = _block_type >> 3;
        _block_type &= 7;

        if (_block_type == 7)
          _block_type = 0;
        else if (_block_type == 5)
          _block_type = 1;
        else if (_block_type == 6)
          _block_type = 2;
        else
          _block_type = 3;
      }
      else
      {
        if (_compressed.read_bits(3)
              .if_ok([&](uintmax_t v) { _block_type = v; })
              .is_err())
          return fail(error_t::out_of_data);
        _final = _block_type & 1;
        _block_type >>= 1;
      }

      if (_block_type == 0)
      {
        // Uncompressed block

        SET_STATE(uncompressed_len)

        if (_compressed.read_bits(16)
              .if_ok([&](uintmax_t v) { _len = v; })
              .is_err())
          return fail(error_t::out_of_data);

        SET_STATE(uncompressed_ilen)

        if (!_anaconda)
        {
          if (_compressed.read_bits(16)
                .if_ok([&](uintmax_t v) { _ilen = v; })
                .is_err())
            return fail(error_t::out_of_data);

          if (_ilen != (~_len & 0xFFFF)) return fail(error_t::corrupt_stream);
        }

        _nread = 0;

        SET_STATE(uncompressed_data)

        while (_nread < _len)
        {
          return lak::ok_or_err(
            _compressed.read_byte()
              .map([this](uintmax_t v) -> lak::deflate_iterator & {
                ++_nread;
                return success(v);
              })
              .map_err([this](...) -> lak::deflate_iterator & {
                return fail(error_t::out_of_data);
              }));
        }

        return block_complete();
      }
      else
      {
        // Compressed block

        if (_block_type == 2)
        {
          // Dynamic tables

          SET_STATE(literal_count)

          if (_compressed.read_bits(5)
                .if_ok([&](uintmax_t v) { _literal_count = v; })
                .is_err())
            return fail(error_t::out_of_data);
          _literal_count += 257;

          SET_STATE(distance_count)

          if (_compressed.read_bits(5)
                .if_ok([&](uintmax_t v) { _distance_count = v; })
                .is_err())
            return fail(error_t::out_of_data);
          _distance_count += 1;

          SET_STATE(codelen_count)

          if (_compressed.read_bits(4)
                .if_ok([&](uintmax_t v) { _codelen_count = v; })
                .is_err())
            return fail(error_t::out_of_data);
          _codelen_count += 4;
          _counter = 0;

          SET_STATE(read_code_lengths)

          if (_anaconda)
          {
            for (; _counter < _codelen_count; ++_counter)
              if (_compressed.read_bits(3)
                    .if_ok([&](uintmax_t v) {
                      _codelen_len[codelen_order_anaconda[_counter]] = v;
                    })
                    .is_err())
                return fail(error_t::out_of_data);

            for (; _counter < 19; ++_counter)
              _codelen_len[codelen_order_anaconda[_counter]] = 0;
          }
          else
          {
            for (; _counter < _codelen_count; ++_counter)
              if (_compressed.read_bits(3)
                    .if_ok([&](uintmax_t v) {
                      _codelen_len[codelen_order[_counter]] = v;
                    })
                    .is_err())
                return fail(error_t::out_of_data);

            for (; _counter < 19; ++_counter)
              _codelen_len[codelen_order[_counter]] = 0;
          }

          if (gen_huffman_table(_codelen_len, false, _codelen_table) !=
              error_t::ok)
            return fail(error_t::huffman_table_gen_failed);

          _repeat_count = 0;
          _last_value   = 0;
          _counter      = 0;

          SET_STATE(read_lengths)

          while (_counter < _literal_count + _distance_count)
          {
            if (_repeat_count == 0)
            {
              if (get_huff(_codelen_table, &_symbol))
                return fail(error_t::out_of_data);

              if (_symbol < 16)
              {
                _last_value   = _symbol;
                _repeat_count = 1;
              }
              else if (_symbol == 16)
              {
                SET_STATE(read_lengths_16)

                if (_compressed.read_bits(2)
                      .if_ok([&](uintmax_t v) { _repeat_count = v; })
                      .is_err())
                  return fail(error_t::out_of_data);
                _repeat_count += 3;
              }
              else if (_symbol == 17)
              {
                _last_value = 0;

                SET_STATE(read_lengths_17)

                if (_compressed.read_bits(3)
                      .if_ok([&](uintmax_t v) { _repeat_count = v; })
                      .is_err())
                  return fail(error_t::out_of_data);
                _repeat_count += 3;
              }
              else if (_symbol != 0)
              {
                _last_value = 0;

                SET_STATE(read_lengths_18)

                if (_compressed.read_bits(7)
                      .if_ok([&](uintmax_t v) { _repeat_count = v; })
                      .is_err())
                  return fail(error_t::out_of_data);
                _repeat_count += 11;
              }
            }

            if (_counter < _literal_count)
              _literal_len[_counter] = uint8_t(_last_value);
            else
              _distance_len[_counter - _literal_count] = uint8_t(_last_value);

            ++_counter;
            --_repeat_count;
            _state = state_t::read_lengths;
          }

          if (gen_huffman_table(lak::span(_literal_len).first(_literal_count),
                                false,
                                _literal_table) != error_t::ok ||
              gen_huffman_table(
                lak::span(_distance_len).first(_distance_count),
                true,
                _distance_table) != error_t::ok)
            return fail(error_t::huffman_table_gen_failed);
        }
        else if (_block_type == 1)
        {
          // Static tables

          int32_t next_free = 2;
          int32_t i         = 0;

          for (; i < 0x7E; ++i)
          {
            _literal_table[i] = ~next_free;
            next_free += 2;
          }

          for (; i < 0x96; ++i)
            _literal_table[i] = uint16_t(i + (0x100 - 0x7E));

          for (; i < 0xFE; ++i)
          {
            _literal_table[i] = ~next_free;
            next_free += 2;
          }

          for (; i < 0x18E; ++i) _literal_table[i] = uint16_t(i - 0xFE);

          for (; i < 0x196; ++i)
            _literal_table[i] = uint16_t(i + (0x118 - 0x18E));

          for (; i < 0x1CE; ++i)
          {
            _literal_table[i] = ~next_free;
            next_free += 2;
          }

          for (; i < 0x23E; ++i)
            _literal_table[i] = uint16_t(i + (0x90 - 0x1CE));

          for (i = 0; i < 0x1E; ++i) _distance_table[i] = ~(i * 2 + 2);

          for (i = 0x1E; i < 0x3E; ++i) _distance_table[i] = i - 0x1E;
        }
        else
          return fail(error_t::invalid_block_code);

        for (;;)
        {
          SET_STATE(read_symbol)

          if (get_huff(_literal_table, &_symbol))
            return fail(error_t::out_of_data);

          if (_symbol < 256) return success(_symbol);

          if (_symbol == 256) break;

          if (_symbol <= 264)
          {
            _repeat_length = (_symbol - 257) + 3;
          }
          else if (_symbol <= 284)
          {
            SET_STATE(read_length)

            const uint32_t length_bits = (_symbol - 261) / 4;
            if (_compressed.read_bits(length_bits)
                  .if_ok([&](uintmax_t v) { _repeat_length = v; })
                  .is_err())
              return fail(error_t::out_of_data);
            _repeat_length += 3 + ((4 + ((_symbol - 265) & 3)) << length_bits);
          }
          else if (_symbol == 285)
          {
            _repeat_length = 258;
          }
          else
          {
            return fail(error_t::invalid_symbol);
          }

          SET_STATE(read_distance)

          if (get_huff(_distance_table, &_symbol))
            return fail(error_t::out_of_data);

          if (_symbol <= 3)
          {
            _distance = _symbol + 1;
          }
          else if (_symbol <= 29)
          {
            SET_STATE(read_distance_extra)

            const uint32_t distance_bits = (_symbol - 2) / 2;
            if (_compressed.read_bits(distance_bits)
                  .if_ok([&](uintmax_t v) { _distance = v; })
                  .is_err())
              return fail(error_t::out_of_data);
            _distance += 1 + ((2 + (_symbol & 1)) << distance_bits);
            if (_distance > _output_buffer.size()) DEBUG_BREAK();
          }
          else
          {
            return fail(error_t::invalid_symbol);
          }

          if (_distance > _output_buffer.size())
            return fail(error_t::invalid_distance);

          SET_STATE(write_repeat);

          while (_repeat_length-- > 0)
            return success(_output_buffer[_output_buffer.size() - _distance]);

          _repeat_length = 0;
        }

        return block_complete();
      }
      break;

    default:
      _value = value_type::make_err(error_t::invalid_state);
      return *this;
  }
#undef SET_STATE
}

template<typename FUNC>
lak::error_code<lak::deflate_iterator::error_t> lak::deflate_iterator::read(
  FUNC &&func)
{
  for (;;)
  {
    step();
    if (_value.is_err())
    {
      const auto err = _value.unsafe_unwrap_err();
      if (err == error_t::ok)
      {
        // reached end of block
        if (_final) break;
      }
      else
        return lak::err_t{err};
    }
    else if (!func(_value.unsafe_unwrap()))
      return lak::err_t{error_t::ok};
  }
  return lak::ok_t{};
}
