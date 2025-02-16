#ifndef LAK_RUNTIME_TYPE_HPP
#define LAK_RUNTIME_TYPE_HPP

#include "lak/array.hpp"
#include "lak/math.hpp"
#include "lak/memory.hpp"
#include "lak/optional.hpp"
#include "lak/string.hpp"

namespace lak
{
	struct runtime_type;
	using runtime_type_ptr = lak::shared_ptr<lak::runtime_type>;

	struct runtime_type
	{
	public:
		enum struct type_kind
		{
			structure,
			function,
			pointer,
			unsigned_integer,
			signed_integer,
			floating_point,
		};

		struct member
		{
			size_t offset;
			lak::u8string name;
			lak::runtime_type_ptr type;
		};

	private:
		size_t _size;
		size_t _alignment;
		type_kind _kind;
		lak::array<member> _members;

		runtime_type() = default;

	public:
		static lak::runtime_type_ptr make_struct(
		  lak::span<const lak::pair<lak::u8string, lak::runtime_type_ptr>> members,
		  bool allow_zst = true);

		static lak::runtime_type_ptr make_function(
		  lak::span<const lak::pair<lak::u8string, lak::runtime_type_ptr>>
		    arguments);

		static lak::runtime_type_ptr make_pointer(lak::runtime_type_ptr to_type);

		static lak::runtime_type_ptr make_signed(uint8_t size);

		static lak::runtime_type_ptr make_unsigned(uint8_t size);

		static lak::runtime_type_ptr make_float(uint8_t size);

		runtime_type(runtime_type &&)            = default;
		runtime_type &operator=(runtime_type &&) = default;

		inline size_t size() const { return _size; }
		inline size_t alignment() const { return _alignment; }
		inline type_kind kind() const { return _kind; }
		inline lak::span<const member> members() const
		{
			return lak::span(_members);
		}
		inline lak::optional<const member &> find_member(
		  lak::u8string_view member_name) const
		{
			for (const auto &mbr : _members)
				if (mbr.name == member_name) return mbr;
			return lak::nullopt;
		}
	};

}

#endif
