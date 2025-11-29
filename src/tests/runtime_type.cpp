#include "lak/test.hpp"

#include "lak/runtime_type.hpp"

#include "lak/string_literals/string.hpp"
#include "lak/string_literals/view.hpp"

BEGIN_TEST(runtime_type)
{
	lak::array<lak::runtime_type_ptr> types;

	types.push_back(lak::runtime_type::make_unsigned(sizeof(uint8_t)));
	types.push_back(lak::runtime_type::make_pointer(types.back()));
	types.push_back(lak::runtime_type::make_struct({
	  {u8"begin"_str, types.back()},
	  {u8"end"_str, types.back()},
	}));
	types.push_back(lak::runtime_type::make_unsigned(sizeof(uint16_t)));
	types.push_back(lak::runtime_type::make_pointer(types.back()));
	types.push_back(lak::runtime_type::make_struct({
	  {u8"begin"_str, types.back()},
	  {u8"end"_str, types.back()},
	}));

	lak::array<lak::array<byte_t>> allocations;

	auto allocate = [&](lak::runtime_type_ptr type,
	                    size_t count) -> lak::span<byte_t>
	{
		auto &alloc = allocations.emplace_back();
		alloc.resize((type->size() * count) + type->alignment());
		return lak::span(lak::align_ptr(alloc.begin(), type->alignment()),
		                 alloc.end())
		  .first(type->size() * count);
	};

	[[maybe_unused]] auto access =
	  [&](lak::runtime_type_ptr type, lak::span<byte_t> data, size_t index)
	{
		ASSERT_GREATER_OR_EQUAL(data.size(), type->size() * index);
		return data.subspan(type->size() * index, type->size());
	};

	auto access_member = [&](lak::runtime_type_ptr type,
	                         lak::span<byte_t> data,
	                         lak::u8string_view member)
	{
		ASSERT_EQUAL(type->size(), data.size());
		auto mbr = type->find_member(member);
		ASSERT(mbr.has_value());
		return data.subspan(mbr->offset, mbr->type->size());
	};

	auto u8block = allocate(types[0], 100U);

	ASSERT_EQUAL(u8block.size(), 100U);

	auto u8span = allocate(types[2], 1U);

	auto u8span_begin = access_member(types[2], u8span, u8"begin"_view);
	ASSERT_EQUAL(u8span_begin.size(), sizeof(byte_t *));
	byte_t *_begin = u8block.begin();
	lak::memcpy(u8span_begin, lak::as_bytes(&_begin));

	auto u8span_end = access_member(types[2], u8span, u8"end"_view);
	ASSERT_EQUAL(u8span_end.size(), sizeof(byte_t *));
	byte_t *_end = u8block.end();
	lak::memcpy(u8span_end, lak::as_bytes(&_end));

	return 0;
}
END_TEST()
