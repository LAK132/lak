#include "lak/runtime_type.hpp"

lak::runtime_type_ptr lak::runtime_type::make_struct(
  lak::span<const lak::pair<lak::u8string, lak::runtime_type_ptr>> members,
  bool allow_zst)
{
	auto result    = lak::runtime_type_ptr::make(runtime_type());
	auto &res      = *result;
	res._size      = 0U;
	res._alignment = 1U;
	res._kind      = type_kind::structure;
	res._members.reserve(members.size());

	for (const auto &[name, type] : members)
	{
		auto &new_mem = res._members.push_back(member{
		  .offset = lak::to_multiple(res._size, type->_alignment),
		  .name   = name,
		  .type   = type,
		});

		res._size      = new_mem.offset + type->_size;
		res._alignment = std::max<size_t>(res._alignment, type->_alignment);
	}

	if (!allow_zst && res._size == 0U) res._size = 1U;

	return result;
}

lak::runtime_type_ptr lak::runtime_type::make_function(
  lak::span<const lak::pair<lak::u8string, lak::runtime_type_ptr>> arguments)
{
	auto result    = lak::runtime_type_ptr::make(runtime_type());
	auto &res      = *result;
	res._size      = sizeof(uintptr_t);
	res._alignment = alignof(uintptr_t);
	res._kind      = type_kind::function;
	res._members.reserve(arguments.size());

	for (size_t i = 0U; const auto &[name, type] : arguments)
	{
		res._members.push_back(member{
		  .offset = i++,
		  .name   = name,
		  .type   = type,
		});
	}

	return result;
}

lak::runtime_type_ptr lak::runtime_type::make_pointer(
  lak::runtime_type_ptr to_type)
{
	auto result    = lak::runtime_type_ptr::make(runtime_type());
	auto &res      = *result;
	res._size      = sizeof(uintptr_t);
	res._alignment = alignof(uintptr_t);
	res._kind      = type_kind::function;

	res._members.push_back(member{
	  .offset = 0U,
	  .name   = {},
	  .type   = to_type,
	});

	return result;
}

lak::runtime_type_ptr lak::runtime_type::make_signed(uint8_t size)
{
	auto result = lak::runtime_type_ptr::make(runtime_type());
	auto &res   = *result;
	res._kind   = type_kind::signed_integer;
	res._size   = size;

	if (size == sizeof(signed char))
		res._alignment = alignof(signed char);
	else if (size == sizeof(signed short))
		res._alignment = alignof(signed short);
	else if (size == sizeof(signed int))
		res._alignment = alignof(signed int);
	else if (size == sizeof(signed long))
		res._alignment = alignof(signed long);
	else if (size == sizeof(signed long long))
		res._alignment = alignof(signed long long);
	else
		ASSERT_UNREACHABLE();

	return result;
}

lak::runtime_type_ptr lak::runtime_type::make_unsigned(uint8_t size)
{
	auto result = lak::runtime_type_ptr::make(runtime_type());
	auto &res   = *result;
	res._kind   = type_kind::unsigned_integer;
	res._size   = size;

	if (size == sizeof(unsigned char))
		res._alignment = alignof(unsigned char);
	else if (size == sizeof(unsigned short))
		res._alignment = alignof(unsigned short);
	else if (size == sizeof(unsigned int))
		res._alignment = alignof(unsigned int);
	else if (size == sizeof(unsigned long))
		res._alignment = alignof(unsigned long);
	else if (size == sizeof(unsigned long long))
		res._alignment = alignof(unsigned long long);
	else
		ASSERT_UNREACHABLE();

	return result;
}

lak::runtime_type_ptr lak::runtime_type::make_float(uint8_t size)
{
	auto result = lak::runtime_type_ptr::make(runtime_type());
	auto &res   = *result;
	res._kind   = type_kind::floating_point;
	res._size   = size;

	if (size == sizeof(float))
		res._alignment = alignof(float);
	else if (size == sizeof(double))
		res._alignment = alignof(double);
	else if (size == sizeof(long double))
		res._alignment = alignof(long double);
	else
		ASSERT_UNREACHABLE();

	return result;
}
