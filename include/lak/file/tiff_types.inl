#include "lak/format_traits.hpp"
#include "lak/stdint.hpp"

#define LAK_FOREACH_TIFF_TYPE(MACRO, ...)                                     \
	MACRO(1, Byte, uint8_t, __VA_ARGS__)                                        \
	MACRO(2, ASCII, char, __VA_ARGS__)                                          \
	MACRO(3, Short, uint16_t, __VA_ARGS__)                                      \
	MACRO(4, Long, uint32_t, __VA_ARGS__)                                       \
	MACRO(5, Rational, lak::tiff::urational, __VA_ARGS__)                       \
	MACRO(6, SByte, int8_t, __VA_ARGS__)                                        \
	MACRO(7, Undefined, byte_t, __VA_ARGS__)                                    \
	MACRO(8, SShort, int16_t, __VA_ARGS__)                                      \
	MACRO(9, SLong, int32_t, __VA_ARGS__)                                       \
	MACRO(10, SRational, lak::tiff::rational, __VA_ARGS__)                      \
	MACRO(11, Float, f32_t, __VA_ARGS__)                                        \
	MACRO(12, Double, f64_t, __VA_ARGS__)                                       \
	MACRO(13, IFD, lak::tiff::_offset, __VA_ARGS__)

namespace lak
{
	namespace tiff
	{
		struct urational
		{
			uint32_t numerator;
			uint32_t denominator;
		};

		struct rational
		{
			int32_t numerator;
			int32_t denominator;
		};

		enum struct _offset : uint32_t
		{
		};

		enum struct tag_type : uint16_t
		{
#define LAK_TIFF_TAG_TYPE(VAL, NAME, ...) NAME = VAL,
			LAK_FOREACH_TIFF_TYPE(LAK_TIFF_TAG_TYPE)
#undef LAK_TIFF_TAG_TYPE
		};
	}

	template<typename CHAR>
	struct format_traits<lak::tiff::urational, CHAR>
	{
		using format_args =
		  typename lak::format_traits<uint32_t, CHAR>::format_args;

		static consteval format_args parse_args(lak::string_view<CHAR> str)
		{
			return lak::format_traits<uint32_t, CHAR>::parse_args(str);
		}

		static constexpr lak::string<CHAR> to_string(
		  const format_args &args, const lak::tiff::urational &val)
		{
			return lak::fmt<CHAR, "{}/{}">(
			  lak::format_traits<uint32_t, CHAR>::to_string(args, val.numerator),
			  lak::format_traits<uint32_t, CHAR>::to_string(args, val.denominator));
		}
	};

	template<typename CHAR>
	struct format_traits<lak::tiff::rational, CHAR>
	{
		using format_args =
		  typename lak::format_traits<int32_t, CHAR>::format_args;

		static consteval format_args parse_args(lak::string_view<CHAR> str)
		{
			return lak::format_traits<int32_t, CHAR>::parse_args(str);
		}

		static constexpr lak::string<CHAR> to_string(
		  const format_args &args, const lak::tiff::rational &val)
		{
			return lak::fmt<CHAR, "{}/{}">(
			  lak::format_traits<int32_t, CHAR>::to_string(args, val.numerator),
			  lak::format_traits<int32_t, CHAR>::to_string(args, val.denominator));
		}
	};

	template<typename CHAR>
	struct format_traits<lak::tiff::_offset, CHAR>
	{
		using format_args =
		  typename lak::format_traits<uint32_t, CHAR>::format_args;

		static consteval format_args parse_args(lak::string_view<CHAR> str)
		{
			return lak::format_traits<uint32_t, CHAR>::parse_args(str);
		}

		static constexpr lak::string<CHAR> to_string(const format_args &args,
		                                             const lak::tiff::_offset &val)
		{
			return lak::format_traits<uint32_t, CHAR>::to_string(
			  args, static_cast<uint32_t>(val));
		}
	};

	template<typename CHAR>
	struct format_traits<lak::tiff::tag_type, CHAR>
	{
		static constexpr lak::string<CHAR> to_string(
		  const lak::tiff::tag_type &tag)
		{
			switch (tag)
			{
#define LAK_TIFF_TAG_TYPE(VAL, NAME, ...)                                     \
	case lak::tiff::tag_type::NAME: return lak::strconv<CHAR>(#NAME ""_view);
				LAK_FOREACH_TIFF_TYPE(LAK_TIFF_TAG_TYPE)
#undef LAK_TIFF_TAG_TYPE
			}
			return {};
		}
	};
}
