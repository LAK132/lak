#include "lak/compression/deflate.hpp"

#include "lak/crc.hpp"

inline lak::deflate_iterator &lak::deflate_iterator::fail(error_t reason)
{
	ASSERT(reason != error_t::ok);
	_crc   = ~_icrc & 0xFFFF'FFFFUL;
	_value = value_type::make_err(reason);
	return *this;
}

inline lak::deflate_iterator &lak::deflate_iterator::success()
{
	ASSERT_GREATER_OR_EQUAL(_output_buffer.size(), _unflushed);
	_value     = value_type::make_ok(_output_buffer.rebase().last(_unflushed));
	_unflushed = 0;
	_crc       = ~_icrc & 0xFFFF'FFFFUL;
	return *this;
}

inline bool lak::deflate_iterator::push_value(byte_t value)
{
	ASSERT_LESS(_unflushed, _output_buffer.max_size());
	++_unflushed;
	_output_buffer.push_back(value);
	_icrc = crc32_table[(_icrc & 0xFF) ^ uint8_t(value)] ^
	        ((_icrc >> 8) & 0xFF'FFFFUL);
	return _unflushed == _output_buffer.max_size();
}

inline lak::deflate_iterator &lak::deflate_iterator::finished()
{
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

	for (;;)
	{
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
					      .if_ok_copy_to<uint8_t>(_block_type)
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
					      .if_ok_copy_to<uint8_t>(_block_type)
					      .is_err())
						return fail(error_t::out_of_data);
					_final = _block_type & 1;
					_block_type >>= 1;
				}

				if (_block_type == 0)
				{
					// Uncompressed block

					SET_STATE(uncompressed_len)

					if (_compressed.read_bits(16).if_ok_copy_to<uint32_t>(_len).is_err())
						return fail(error_t::out_of_data);

					SET_STATE(uncompressed_ilen)

					if (!_anaconda)
					{
						if (_compressed.read_bits(16)
						      .if_ok_copy_to<uint32_t>(_ilen)
						      .is_err())
							return fail(error_t::out_of_data);

						if (_ilen != (~_len & 0xFFFF))
							return fail(error_t::corrupt_stream);
					}

					_nread = 0;

					SET_STATE(uncompressed_data)

					while (_nread < _len)
					{
						auto result = _compressed.read_byte();
						if_let_ok (byte_t v, result)
						{
							++_nread;
							if (push_value(v)) return success();
						}
						if_let_err ([[maybe_unused]] const auto err, result)
						{
							return fail(error_t::out_of_data);
						}
					}
				}
				else
				{
					// Compressed block

					if (_block_type == 2)
					{
						// Dynamic tables

						SET_STATE(literal_count)

						if (_compressed.read_bits(5)
						      .if_ok_copy_to<uint32_t>(_literal_count)
						      .is_err())
							return fail(error_t::out_of_data);
						_literal_count += 257;

						SET_STATE(distance_count)

						if (_compressed.read_bits(5)
						      .if_ok_copy_to<uint32_t>(_distance_count)
						      .is_err())
							return fail(error_t::out_of_data);
						_distance_count += 1;

						SET_STATE(codelen_count)

						if (_compressed.read_bits(4)
						      .if_ok_copy_to<uint32_t>(_codelen_count)
						      .is_err())
							return fail(error_t::out_of_data);
						_codelen_count += 4;
						_counter = 0;

						SET_STATE(read_code_lengths)

						if (_anaconda)
						{
							for (; _counter < _codelen_count; ++_counter)
								if (_compressed.read_bits(3)
								      .if_ok_copy_to<uint8_t>(
								        _codelen_len[codelen_order_anaconda[_counter]])
								      .is_err())
									return fail(error_t::out_of_data);

							for (; _counter < 19; ++_counter)
								_codelen_len[codelen_order_anaconda[_counter]] = 0;
						}
						else
						{
							for (; _counter < _codelen_count; ++_counter)
								if (_compressed.read_bits(3)
								      .if_ok_copy_to<uint8_t>(
								        _codelen_len[codelen_order[_counter]])
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
									      .if_ok_copy_to<uint32_t>(_repeat_count)
									      .is_err())
										return fail(error_t::out_of_data);
									_repeat_count += 3;
								}
								else if (_symbol == 17)
								{
									_last_value = 0;

									SET_STATE(read_lengths_17)

									if (_compressed.read_bits(3)
									      .if_ok_copy_to<uint32_t>(_repeat_count)
									      .is_err())
										return fail(error_t::out_of_data);
									_repeat_count += 3;
								}
								else if (_symbol != 0)
								{
									_last_value = 0;

									SET_STATE(read_lengths_18)

									if (_compressed.read_bits(7)
									      .if_ok_copy_to<uint32_t>(_repeat_count)
									      .is_err())
										return fail(error_t::out_of_data);
									_repeat_count += 11;
								}
							}

							if (_counter < _literal_count)
								_literal_len[_counter] = uint8_t(_last_value);
							else
								_distance_len[_counter - _literal_count] =
								  uint8_t(_last_value);

							++_counter;
							--_repeat_count;
							_state = state_t::read_lengths;
						}

						if (gen_huffman_table(
						      lak::span(_literal_len).first(_literal_count),
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
							_literal_table[i] = uint16_t(~next_free);
							next_free += 2;
						}

						for (; i < 0x96; ++i)
							_literal_table[i] = uint16_t(i + (0x100 - 0x7E));

						for (; i < 0xFE; ++i)
						{
							_literal_table[i] = uint16_t(~next_free);
							next_free += 2;
						}

						for (; i < 0x18E; ++i) _literal_table[i] = uint16_t(i - 0xFE);

						for (; i < 0x196; ++i)
							_literal_table[i] = uint16_t(i + (0x118 - 0x18E));

						for (; i < 0x1CE; ++i)
						{
							_literal_table[i] = uint16_t(~next_free);
							next_free += 2;
						}

						for (; i < 0x23E; ++i)
							_literal_table[i] = uint16_t(i + (0x90 - 0x1CE));

						for (i = 0; i < 0x1E; ++i)
							_distance_table[i] = uint16_t(~(i * 2 + 2));

						for (i = 0x1E; i < 0x3E; ++i)
							_distance_table[i] = uint16_t(i - 0x1E);
					}
					else
						return fail(error_t::invalid_block_code);

					for (;;)
					{
						SET_STATE(read_symbol)

						if (get_huff(_literal_table, &_symbol))
							return fail(error_t::out_of_data);

						if (_symbol < 256)
						{
							if (push_value(byte_t(_symbol)))
								return success();
							else
								continue;
						}

						if (_symbol == 256) break;

						if (_symbol <= 264)
						{
							_repeat_length = (_symbol - 257) + 3;
						}
						else if (_symbol <= 284)
						{
							SET_STATE(read_length)

							const uint8_t length_bits = uint8_t((_symbol - 261) / 4);
							if (_compressed.read_bits(length_bits)
							      .if_ok_copy_to<uint32_t>(_repeat_length)
							      .is_err())
								return fail(error_t::out_of_data);
							_repeat_length +=
							  3 + ((4 + ((_symbol - 265) & 3)) << length_bits);
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

							const uint8_t distance_bits = uint8_t((_symbol - 2) / 2);
							if (_compressed.read_bits(distance_bits)
							      .if_ok_copy_to<uint32_t>(_distance)
							      .is_err())
								return fail(error_t::out_of_data);
							_distance += 1 + ((2 + (_symbol & 1)) << distance_bits);
						}
						else
						{
							return fail(error_t::invalid_symbol);
						}

						if (_distance > _output_buffer.size())
							return fail(error_t::invalid_distance);

						SET_STATE(write_repeat);

						while (_repeat_length-- > 0)
							if (push_value(
							      _output_buffer[_output_buffer.size() - _distance]))
								return success();

						_repeat_length = 0;
					}
				}

				SET_STATE(block_complete)

				if (!_final)
					_state = state_t::header;
				else if (_unflushed > 0)
					return success();
				else
					return finished();
				break;

			default:
				_value = value_type::make_err(error_t::invalid_state);
				return *this;
		}
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
			if (auto err = _value.unsafe_unwrap_err(); err == error_t::ok)
			{
				// reached end of block

				if (_final) return lak::ok_t{};
			}
			else
				return lak::err_t{err};
		}
		else if (!func(_value.unsafe_unwrap()))
			return lak::err_t{error_t::ok};
	}
}
