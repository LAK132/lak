#include "lak/ebpf/vm.hpp"

#include "lak/memmanip.hpp"
#include "lak/span_manip.hpp"
#include "lak/system/endian.hpp"

lak::result<uint64_t, size_t> lak::ebpf::vm::run_program(
  lak::span<lak::ebpf::instruction_t> program,
  uint64_t r1init,
  uint64_t r2init,
  uint64_t r3init,
  uint64_t r4init,
  uint64_t r5init)
{
	lak::array<uint64_t, 6U> r0r5 = {0, r1init, r2init, r3init, r4init, r5init};
	lak::array<lak::pair<size_t, lak::array<uint64_t, 4U>>> stack_r6r9;
	lak::array<byte_t> stack;
	stack.resize(stack_size);
	uint64_t r10 = stack_size;

	stack_r6r9.push_back({program.size(), lak::array<uint64_t, 4U>{0, 0, 0, 0}});

	size_t pc = 0U;

	auto register_store =
	  [&](register_t reg, uint64_t val, opcode_load_store_size size)
	{
		switch (size)
		{
			using enum opcode_load_store_size;
			case DW: break;
			case W:  val &= UINT32_MAX; break;
			case H:  val &= UINT16_MAX; break;
			case B:  val &= UINT8_MAX; break;
		}
		switch (reg)
		{
			case register_t::R0:
			case register_t::R1:
			case register_t::R2:
			case register_t::R3:
			case register_t::R4:
			case register_t::R5: r0r5[static_cast<size_t>(reg)] = val; break;
			case register_t::R6:
			case register_t::R7:
			case register_t::R8:
			case register_t::R9:
				stack_r6r9.back().second[static_cast<size_t>(reg) - 6U] = val;
				break;
			default: FATAL("invalid register"); break;
		}
	};

	auto register_read = [&](register_t reg,
	                         opcode_load_store_size size) -> uint64_t
	{
		uint64_t val;
		switch (reg)
		{
			case register_t::R0:
			case register_t::R1:
			case register_t::R2:
			case register_t::R3:
			case register_t::R4:
			case register_t::R5: val = r0r5[static_cast<size_t>(reg)]; break;
			case register_t::R6:
			case register_t::R7:
			case register_t::R8:
			case register_t::R9:
				val = stack_r6r9.back().second[static_cast<size_t>(reg) - 6U];
				break;
			case register_t::R10: val = r10; break;
			default:              FATAL("invalid register"); break;
		}
		switch (size)
		{
			using enum opcode_load_store_size;
			case DW: break;
			case W:  val &= UINT32_MAX; break;
			case H:  val &= UINT16_MAX; break;
			case B:  val &= UINT8_MAX; break;
		}
		return val;
	};

	auto memory_store =
	  [&](int16_t offset, uint64_t val, opcode_load_store_size size)
	{
		if (static_cast<int64_t>(offset) < -static_cast<int64_t>(r10))
			FATAL("stack overflow");
		switch (size)
		{
			using enum opcode_load_store_size;
			case B:
				if (static_cast<int64_t>(offset) > -1) FATAL("stack overflow");
				*reinterpret_cast<uint8_t *>(stack.end() + offset) =
				  static_cast<uint8_t>(val);
				break;
			case H:
				if (static_cast<int64_t>(offset) > -2) FATAL("stack overflow");
				*reinterpret_cast<uint16_t *>(stack.end() + offset) =
				  static_cast<uint16_t>(val);
				break;
			case W:
				if (static_cast<int64_t>(offset) > -4) FATAL("stack overflow");
				*reinterpret_cast<uint32_t *>(stack.end() + offset) =
				  static_cast<uint32_t>(val);
				break;
			case DW:
				if (static_cast<int64_t>(offset) > -8) FATAL("stack overflow");
				*reinterpret_cast<uint64_t *>(stack.end() + offset) =
				  static_cast<uint64_t>(val);
				break;
			default: FATAL("invalid size"); break;
		}
	};

	auto memory_read = [&](int16_t offset,
	                       opcode_load_store_size size) -> uint64_t
	{
		if (static_cast<int64_t>(offset) < -static_cast<int64_t>(r10))
			FATAL("stack overflow");
		switch (size)
		{
			using enum opcode_load_store_size;
			case B:
				if (static_cast<int64_t>(offset) > -1) FATAL("stack overflow");
				return *reinterpret_cast<uint8_t *>(stack.end() + offset);
				break;
			case H:
				if (static_cast<int64_t>(offset) > -2) FATAL("stack overflow");
				return *reinterpret_cast<uint16_t *>(stack.end() + offset);
				break;
			case W:
				if (static_cast<int64_t>(offset) > -4) FATAL("stack overflow");
				return *reinterpret_cast<uint32_t *>(stack.end() + offset);
				break;
			case DW:
				if (static_cast<int64_t>(offset) > -8) FATAL("stack overflow");
				return *reinterpret_cast<uint64_t *>(stack.end() + offset);
				break;
			default: FATAL("invalid size"); break;
		}
	};

	auto decode_next = [&]()
	{ return lak::ebpf::instruction::make(program[pc++]); };

	static_assert(sizeof(uint64_t) >= sizeof(uintptr_t));

	auto map_by_fd = [&](uint32_t fd) -> uint64_t
	{
		LAK_UNUSED(fd);
		ASSERT_NYI();
	};
	auto map_by_idx = [&](uint32_t idx) -> uint64_t
	{
		LAK_UNUSED(idx);
		ASSERT_NYI();
	};
	auto variable_addr = [&](uint32_t id) -> uint64_t
	{
		LAK_UNUSED(id);
		ASSERT_NYI();
	};
	auto mva = [&](uint64_t map) -> uint64_t
	{
		LAK_UNUSED(map);
		ASSERT_NYI();
	};
	auto code_addr = [&](uint32_t idx) -> uint64_t
	{ return reinterpret_cast<uint64_t>(program.begin() + idx); };

	for (;;)
	{
		ASSERT_GREATER(program.size(), pc);

		auto inst = decode_next();

		switch (inst.opcode_class)
		{
			using enum opcode_class_t;

			case LD:
			{
				using enum opcode_load_store_mode;
				using enum opcode_load_store_size;

				switch (inst.mode())
				{
					case IMM:
					{
						uint64_t val = 0U;
						switch (inst.src)
						{
							case 0U:
								val = static_cast<uint64_t>(decode_next().immediate) << 32U |
								      inst.immediate;
								break;
							case 1U: val = map_by_fd(inst.immediate); break;
							case 2U:
								val = mva(map_by_fd(inst.immediate)) + decode_next().immediate;
								break;
							case 3U: val = variable_addr(inst.immediate); break;
							case 4U: val = code_addr(inst.immediate); break;
							case 5U: val = map_by_idx(inst.immediate); break;
							case 6U:
								val =
								  mva(map_by_idx(inst.immediate)) + decode_next().immediate;
								break;
							default: FATAL("invalid source"); break;
						}
						register_store(inst.dst_reg(), val, inst.size());
					}
					break;

					default: FATAL("invalid opcode"); break;
				}
			}
			break;

			case LDX:
			{
				using enum opcode_load_store_mode;
				using enum opcode_load_store_size;
				switch (inst.mode())
				{
					case MEM:
						if (inst.src_reg() != register_t::R10)
							FATAL("invalid memory read register");
						register_store(
						  inst.dst_reg(),
						  memory_read(static_cast<int16_t>(inst.offset), inst.size()),
						  inst.size());
						break;

					case MEMSX:
					{
						if (inst.src_reg() != register_t::R10)
							FATAL("invalid memory read register");
						uint64_t mem =
						  memory_read(static_cast<int16_t>(inst.offset), inst.size());
						switch (inst.size())
						{
							case W:
								mem = static_cast<uint64_t>(static_cast<int64_t>(
								  static_cast<int32_t>(static_cast<uint32_t>(mem))));
								break;
							case H:
								mem = static_cast<uint64_t>(static_cast<int64_t>(
								  static_cast<int16_t>(static_cast<uint16_t>(mem))));
								break;
							case B:
								mem = static_cast<uint64_t>(static_cast<int64_t>(
								  static_cast<int8_t>(static_cast<uint8_t>(mem))));
								break;
							default: FATAL("invalid size"); break;
						}
						register_store(inst.dst_reg(), mem, inst.size());
					}
					break;

					default: FATAL("invalid opcode"); break;
				}
			}
			break;

			case ST:
			{
				using enum opcode_load_store_mode;
				using enum opcode_load_store_size;

				switch (inst.mode())
				{
					case MEM:
					{
						if (inst.dst_reg() != register_t::R10)
							FATAL("invalid memory read register");
						memory_store(
						  static_cast<int16_t>(inst.offset), inst.immediate, inst.size());
					}
					break;

					default: FATAL("invalid opcode"); break;
				}
			}
			break;

			case STX:
			{
				using enum opcode_load_store_mode;
				using enum opcode_load_store_size;

				switch (inst.mode())
				{
					case MEM:
					{
						if (inst.dst_reg() != register_t::R10)
							FATAL("invalid memory read register");
						memory_store(static_cast<int16_t>(inst.offset),
						             register_read(inst.src_reg(), DW),
						             inst.size());
					}
					break;

					case ATOMIC:
					{
						switch (inst.atomic_code())
						{
							using enum opcode_atomic;

							case FETCH_ADD:
							{
							}
								[[fallthrough]];
							case ADD:
							{
								ASSERT_NYI();
							}
							break;

							case FETCH_OR:
							{
							}
								[[fallthrough]];
							case OR:
							{
								ASSERT_NYI();
							}
							break;

							case FETCH_AND:
							{
							}
								[[fallthrough]];
							case AND:
							{
								ASSERT_NYI();
							}
							break;

							case FETCH_XOR:
							{
							}
								[[fallthrough]];
							case XOR:
							{
								ASSERT_NYI();
							}
							break;

							case XCHG:
							{
								ASSERT_NYI();
							}
							break;

							case CMPXCHG:
							{
								ASSERT_NYI();
							}
							break;

							default: FATAL("invalid opcode"); break;
						}
					}
					break;

					default: FATAL("invalid opcode"); break;
				}
			}
			break;

			case ALU:
			{
				using enum opcode_alu_jump_source;
				using enum opcode_alu;
				using enum opcode_alu_endian;
				using enum opcode_load_store_size;

				uint32_t _dst =
				  static_cast<uint32_t>(register_read(inst.dst_reg(), W));

				if (inst.alu_code() == END)
				{
					static_assert(lak::endian::native == lak::endian::little ||
					              lak::endian::native == lak::endian::big);

					switch (inst.endian())
					{
						case TO_LE:
							if constexpr (lak::endian::native != lak::endian::little)
								switch (inst.immediate)
								{
									case 16U: lak::byte_swap<2U>(lak::as_bytes(&_dst)); break;
									case 32U: lak::byte_swap<4U>(lak::as_bytes(&_dst)); break;
									case 64U: lak::byte_swap<8U>(lak::as_bytes(&_dst)); break;
									default:  FATAL("invalid opcode"); break;
								}
							break;

						case TO_BE:
							if constexpr (lak::endian::native != lak::endian::big)
								switch (inst.immediate)
								{
									case 16U: lak::byte_swap<2U>(lak::as_bytes(&_dst)); break;
									case 32U: lak::byte_swap<4U>(lak::as_bytes(&_dst)); break;
									case 64U: lak::byte_swap<8U>(lak::as_bytes(&_dst)); break;
									default:  FATAL("invalid opcode"); break;
								}
							break;

						default: FATAL("invalid opcode"); break;
					}
				}
				else
				{
					uint32_t _src = static_cast<uint32_t>(
					  inst.source() == K ? inst.immediate
					                     : register_read(inst.src_reg(), W));

					switch (inst.alu_code())
					{
						case ADD: _dst += _src; break;
						case SUB: _dst -= _src; break;
						case MUL: _dst *= _src; break;
						case DIV:
							if (inst.offset == 1)
								_dst = _src != 0U ? static_cast<int32_t>(_dst) /
								                      static_cast<int32_t>(_src)
								                  : 0U;
							else
								_dst = _src != 0U ? _dst / _src : 0U;
							break;
						case OR:  _dst |= _src; break;
						case AND: _dst &= _src; break;
						case LSH: _dst <<= _src; break;
						case RSH: _dst >>= _src; break;
						case NEG:
							_dst = static_cast<uint32_t>(-static_cast<int32_t>(_src));
							break;
						case MOD:
							if (inst.offset == 1)
								_dst = _src != 0U ? static_cast<int32_t>(_dst) %
								                      static_cast<int32_t>(_src)
								                  : _dst;
							else
								_dst = _src != 0U ? _dst % _src : _dst;
							break;
						case XOR: _dst ^= _src; break;
						case MOV:
							if (inst.source() == X)
								_dst = _src;
							else
								switch (inst.offset)
								{
									case 8:
										_dst = static_cast<uint32_t>(static_cast<int32_t>(
										  static_cast<int8_t>(static_cast<uint8_t>(_src))));
										break;
									case 16:
										_dst = static_cast<uint32_t>(static_cast<int32_t>(
										  static_cast<int16_t>(static_cast<uint16_t>(_src))));
										break;
									case 32:
										_dst = static_cast<uint32_t>(static_cast<int32_t>(
										  static_cast<int32_t>(static_cast<uint32_t>(_src))));
										break;
									default: _dst = _src; break;
								}
							break;
						case ARSH:
							_dst = (_dst >> _src) | ((~((_dst & (1U << 31U)) >> _src)) + 1U);
							break;
						default: FATAL("invalid opcode"); break;
					}
				}

				register_store(inst.dst_reg(), _dst, DW);
			}
			break;

			case ALU64:
			{
				using enum opcode_alu_jump_source;
				using enum opcode_alu;
				using enum opcode_load_store_size;
				uint64_t _dst = register_read(inst.dst_reg(), DW);

				if (inst.alu_code() == END)
				{
					switch (inst.immediate)
					{
						case 16U: lak::byte_swap<2U>(lak::as_bytes(&_dst)); break;
						case 32U: lak::byte_swap<4U>(lak::as_bytes(&_dst)); break;
						case 64U: lak::byte_swap<8U>(lak::as_bytes(&_dst)); break;
						default:  FATAL("invalid opcode"); break;
					}
					break;
				}
				else
				{
					uint64_t _src = inst.source() == K
					                  ? inst.immediate
					                  : register_read(inst.src_reg(), DW);

					switch (inst.alu_code())
					{
						case ADD: _dst += _src; break;
						case SUB: _dst -= _src; break;
						case MUL: _dst *= _src; break;
						case DIV:
							if (inst.offset == 1)
								_dst = _src != 0U ? static_cast<int64_t>(_dst) /
								                      static_cast<int64_t>(_src)
								                  : 0U;
							else
								_dst = _src != 0U ? _dst / _src : 0U;
							break;
						case OR:  _dst |= _src; break;
						case AND: _dst &= _src; break;
						case LSH: _dst <<= _src; break;
						case RSH: _dst >>= _src; break;
						case NEG:
							_dst = static_cast<uint64_t>(-static_cast<int64_t>(_src));
							break;
						case MOD:
							if (inst.offset == 1)
								_dst = _src != 0U ? static_cast<int64_t>(_dst) %
								                      static_cast<int64_t>(_src)
								                  : _dst;
							else
								_dst = _src != 0U ? _dst % _src : _dst;
							break;
						case XOR: _dst ^= _src; break;
						case MOV:
							if (inst.source() == X)
								_dst = _src;
							else
								switch (inst.offset)
								{
									case 8:
										_dst = static_cast<uint64_t>(static_cast<int32_t>(
										  static_cast<int8_t>(static_cast<uint8_t>(_src))));
										break;
									case 16:
										_dst = static_cast<uint64_t>(static_cast<int32_t>(
										  static_cast<int16_t>(static_cast<uint16_t>(_src))));
										break;
									case 32:
										_dst = static_cast<uint64_t>(static_cast<int32_t>(
										  static_cast<int32_t>(static_cast<uint32_t>(_src))));
										break;
									default: _dst = _src; break;
								}
							break;
							break;
						case ARSH:
							_dst = (_dst >> _src) | ((~((_dst & (1U << 31U)) >> _src)) + 1U);
							break;
						default: FATAL("invalid opcode"); break;
					}
				}

				register_store(inst.dst_reg(), _dst, DW);
			}
			break;

			case JMP:
			{
				using enum opcode_alu_jump_source;
				using enum opcode_jump;
				using enum opcode_jump_call;
				using enum opcode_load_store_size;

				bool is_conditional = false;
				switch (inst.jump_code())
				{
					case JA: pc += static_cast<int16_t>(inst.offset); break;
					case CALL:
					{
						switch (inst.call_src())
						{
							case AGNOSTIC:
								agnostic_helper[inst.immediate](this,
								                                r0r5[0],
								                                r0r5[1],
								                                r0r5[2],
								                                r0r5[3],
								                                r0r5[4],
								                                r0r5[5],
								                                stack_r6r9.back().second[0],
								                                stack_r6r9.back().second[1],
								                                stack_r6r9.back().second[2],
								                                stack_r6r9.back().second[3],
								                                r10);
								break;
							case LOCAL:
								stack_r6r9.push_back({pc, stack_r6r9.back().second});
								pc += static_cast<int32_t>(inst.immediate);
								break;
							case SPECIFIC:
								specific_helper[inst.immediate](this,
								                                r0r5[0],
								                                r0r5[1],
								                                r0r5[2],
								                                r0r5[3],
								                                r0r5[4],
								                                r0r5[5],
								                                stack_r6r9.back().second[0],
								                                stack_r6r9.back().second[1],
								                                stack_r6r9.back().second[2],
								                                stack_r6r9.back().second[3],
								                                r10);
								break;
							default: FATAL("invalid opcode"); break;
						}
					}
					break;
					case EXIT:
					{
						pc = stack_r6r9.back().first;
						stack_r6r9.pop_back();
						if (stack_r6r9.empty())
						{
							return lak::ok_t{r0r5[0]};
						}
					}
					break;
					default: is_conditional = true; break;
				}
				if (!is_conditional) break;
				uint64_t _dst = register_read(inst.dst_reg(), DW);
				uint64_t _src = inst.source() == K
				                  ? static_cast<uint64_t>(static_cast<int64_t>(
				                      static_cast<int32_t>(inst.immediate)))
				                  : register_read(inst.src_reg(), DW);
				switch (inst.jump_code())
				{
					case JEQ:
						if (_dst == _src) pc += static_cast<int16_t>(inst.offset);
						break;
					case JGT:
						if (_dst > _src) pc += static_cast<int16_t>(inst.offset);
						break;
					case JGE:
						if (_dst >= _src) pc += static_cast<int16_t>(inst.offset);
						break;
					case JSET:
						if (_dst & _src) pc += static_cast<int16_t>(inst.offset);
						break;
					case JNE:
						if (_dst != _src) pc += static_cast<int16_t>(inst.offset);
						break;
					case JSGT:
						if (static_cast<int64_t>(_dst) > static_cast<int64_t>(_src))
							pc += static_cast<int16_t>(inst.offset);
						break;
					case JSGE:
						if (static_cast<int64_t>(_dst) >= static_cast<int64_t>(_src))
							pc += static_cast<int16_t>(inst.offset);
						break;
					case JLT:
						if (_dst < _src) pc += static_cast<int16_t>(inst.offset);
						break;
					case JLE:
						if (_dst < _src) pc += static_cast<int16_t>(inst.offset);
						break;
					case JSLT:
						if (static_cast<int64_t>(_dst) < static_cast<int64_t>(_src))
							pc += static_cast<int16_t>(inst.offset);
						break;
					case JSLE:
						if (static_cast<int64_t>(_dst) <= static_cast<int64_t>(_src))
							pc += static_cast<int16_t>(inst.offset);
						break;
					default: FATAL("invalid opcode"); break;
				}
			}
			break;

			case JMP32:
			{
				using enum opcode_alu_jump_source;
				using enum opcode_jump;
				using enum opcode_load_store_size;

				uint32_t _dst =
				  static_cast<uint32_t>(register_read(inst.dst_reg(), W));
				uint32_t _src = static_cast<uint32_t>(
				  inst.source() == K ? inst.immediate
				                     : register_read(inst.src_reg(), W));
				switch (inst.jump_code())
				{
					case JA:
						if (inst.source() != K) FATAL("invalid opcode");
						pc += inst.immediate;
						break;
					case JEQ:
						if (_dst == _src) pc += static_cast<int16_t>(inst.offset);
						break;
					case JGT:
						if (_dst > _src) pc += static_cast<int16_t>(inst.offset);
						break;
					case JGE:
						if (_dst >= _src) pc += static_cast<int16_t>(inst.offset);
						break;
					case JSET:
						if (_dst & _src) pc += static_cast<int16_t>(inst.offset);
						break;
					case JNE:
						if (_dst != _src) pc += static_cast<int16_t>(inst.offset);
						break;
					case JSGT:
						if (static_cast<int32_t>(_dst) > static_cast<int32_t>(_src))
							pc += static_cast<int16_t>(inst.offset);
						break;
					case JSGE:
						if (static_cast<int32_t>(_dst) >= static_cast<int32_t>(_src))
							pc += static_cast<int16_t>(inst.offset);
						break;
					case JLT:
						if (_dst < _src) pc += static_cast<int16_t>(inst.offset);
						break;
					case JLE:
						if (_dst < _src) pc += static_cast<int16_t>(inst.offset);
						break;
					case JSLT:
						if (static_cast<int32_t>(_dst) < static_cast<int32_t>(_src))
							pc += static_cast<int16_t>(inst.offset);
						break;
					case JSLE:
						if (static_cast<int32_t>(_dst) <= static_cast<int32_t>(_src))
							pc += static_cast<int16_t>(inst.offset);
						break;
					default: FATAL("invalid opcode"); break;
				}
			}
			break;

			default: FATAL("invalid opcode"); break;
		}
	}
}
