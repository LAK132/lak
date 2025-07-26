#ifndef LAK_STRUCTURE_TIFF_HPP
#define LAK_STRUCTURE_TIFF_HPP

#include "lak/binary_reader.hpp"
#include "lak/binary_writer.hpp"
#include "lak/math.hpp"
#include "lak/span.hpp"
#include "lak/stdint.hpp"
#include "lak/string_literals.hpp"
#include "lak/type_pack.hpp"
#include "lak/utility.hpp"
#include "lak/variant.hpp"

#include "tiff_tags.inl"

namespace lak
{
	namespace tiff
	{
		struct image_file_directory;
	}

	template<>
	struct _array_type_is_copyable<lak::tiff::image_file_directory>
	: lak::true_type
	{
	};

	namespace tiff
	{
		template<typename T = lak::monostate>
		using result = lak::result<
		  T,
		  lak::variant<lak::out_of_data_error, lak::value_out_of_range_error>>;

		struct urational
		{
			uint32_t numerator;
			uint32_t denominator;
		};
		inline std::ostream &operator<<(std::ostream &strm,
		                                const lak::tiff::urational &rational)
		{
			return strm << rational.numerator << "/" << rational.denominator;
		}

		struct rational
		{
			int32_t numerator;
			int32_t denominator;
		};
		inline std::ostream &operator<<(std::ostream &strm,
		                                const lak::tiff::rational &rational)
		{
			return strm << rational.numerator << "/" << rational.denominator;
		}

		enum struct _offset : uint32_t
		{
		};
		inline std::ostream &operator<<(std::ostream &strm,
		                                const lak::tiff::_offset &off)
		{
			return strm << static_cast<uint32_t>(off);
		}

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

		enum struct tag_type : uint16_t
		{
#define LAK_TIFF_TAG_TYPE(VAL, NAME, ...) NAME = VAL,
			LAK_FOREACH_TIFF_TYPE(LAK_TIFF_TAG_TYPE)
#undef LAK_TIFF_TAG_TYPE
		};

		using tag_types_pack =
		  lak::remove_from_pack_t<lak::bottom,
		                          lak::type_pack<
#define LAK_TIFF_TAG_TYPE(VAL, NAME, TYPE, ...) TYPE,
		                            LAK_FOREACH_TIFF_TYPE(LAK_TIFF_TAG_TYPE)
#undef LAK_TIFF_TAG_TYPE
		                              lak::bottom>>;

		enum struct tag_name : uint16_t
		{
#define LAK_TIFF_TAG_NAME(VAL, NAME, ...) NAME = VAL,
			LAK_FOREACH_TIFF_TAG_VALUE(LAK_TIFF_TAG_NAME)
#undef LAK_TIFF_TAG_NAME
		};

		inline std::ostream &operator<<(std::ostream &strm,
		                                const lak::tiff::tag_type &tag)
		{
			switch (tag)
			{
#define LAK_TIFF_TAG_TYPE(VAL, NAME, ...)                                     \
	case lak::tiff::tag_type::NAME:                                             \
		strm << #NAME;                                                            \
		break;
				LAK_FOREACH_TIFF_TYPE(LAK_TIFF_TAG_TYPE)
#undef LAK_TIFF_TAG_TYPE
			}
			return strm;
		}

		inline std::ostream &operator<<(std::ostream &strm,
		                                const lak::tiff::tag_name &tag)
		{
			switch (tag)
			{
#define LAK_TIFF_TAG_NAME(VAL, NAME, ...)                                     \
	case lak::tiff::tag_name::NAME:                                             \
		strm << #NAME;                                                            \
		break;
				LAK_FOREACH_TIFF_TAG_VALUE(LAK_TIFF_TAG_NAME)
#undef LAK_TIFF_TAG_NAME
			}
			return strm;
		}

	}
}

LAK_FIXED_STRUCT_BYTES_TRAITS(lak::tiff::urational,
                              &lak::tiff::urational::numerator,
                              &lak::tiff::urational::denominator);
LAK_FIXED_STRUCT_BYTES_TRAITS(lak::tiff::rational,
                              &lak::tiff::rational::numerator,
                              &lak::tiff::rational::denominator);
LAK_MEMCPY_BYTE_TRAITS_IMPL(lak::tiff::_offset);
LAK_MEMCPY_BYTE_TRAITS_IMPL(lak::tiff::tag_type);
LAK_MEMCPY_BYTE_TRAITS_IMPL(lak::tiff::tag_name);

namespace lak
{
	namespace tiff
	{
		struct image_file_header
		{
			uint16_t version;
		};

		struct strip
		{
			lak::array<byte_t> data;
		};

		struct tile
		{
			lak::array<byte_t> data;
		};

		struct ifd_tag
		{
			lak::tiff::tag_name id;
			lak::array<byte_t, 4U> _value;
			lak::array<byte_t> _data_store;
			lak::create_from_pack_t<
			  lak::variant,
			  lak::pack_foreach_t<lak::dynamic_span, lak::tiff::tag_types_pack>>
			  data = lak::span<byte_t>{};

			ifd_tag() = default;
			inline ifd_tag(ifd_tag &&other);
			inline ifd_tag &operator=(ifd_tag &&other);

			template<typename T>
			requires(
#define LAK_TIFF_IFD_TAG(VAL, NAME, TYPE, ...) lak::is_same_v<T, TYPE> ||
			  LAK_FOREACH_TIFF_TYPE(LAK_TIFF_IFD_TAG)
#undef LAK_TIFF_IFD_TAG
			    false)
			void set_data(lak::array<T> &&new_data);

			template<typename T>
			requires(
#define LAK_TIFF_IFD_TAG(VAL, NAME, TYPE, ...) lak::is_same_v<T, TYPE> ||
			  LAK_FOREACH_TIFF_TYPE(LAK_TIFF_IFD_TAG)
#undef LAK_TIFF_IFD_TAG
			    false)
			void set_data(lak::span<const T> new_data);

			auto visit(auto &&func) const { return data.visit(func); }

			template<lak::endian E>
			lak::tiff::result<> read(lak::binary_reader &strm);

			inline size_t write_size() const;

			template<lak::endian E>
			lak::tiff::result<> write(lak::binary_span_writer &strm,
			                          lak::binary_span_writer &ext_strm) const;

#define LAK_TIFF_TAG_MAKE(NAME, TYPE, COUNT, ...)                             \
	static ifd_tag make_##NAME(lak::span<const TYPE, COUNT> data)               \
	{                                                                           \
		ifd_tag result{};                                                         \
		result.id = lak::tiff::tag_name::NAME;                                    \
		result.template set_data<TYPE>(data);                                     \
		return result;                                                            \
	}                                                                           \
	static ifd_tag make_##NAME(lak::array<TYPE, COUNT> &&data)                  \
	{                                                                           \
		ifd_tag result{};                                                         \
		result.id = lak::tiff::tag_name::NAME;                                    \
		result.template set_data<TYPE>(lak::move(data));                          \
		return result;                                                            \
	}
			LAK_FOREACH_TIFF_TAG_TYPES(LAK_TIFF_TAG_MAKE)
#undef LAK_TIFF_TAG_MAKE
		};

		struct image_file_directory
		{
			uint32_t rows;
			lak::array<lak::tiff::strip> strips;
			lak::array<lak::tiff::tile> tiles;
			lak::array<lak::tiff::ifd_tag> tags;
			lak::array<image_file_directory> subifds;
			uint32_t _ifd_offset;

			template<lak::endian E>
			lak::tiff::result<> read(lak::binary_reader &strm);

			inline size_t write_size() const;

			template<lak::endian E>
			lak::tiff::result<> write(lak::binary_span_writer &strm,
			                          lak::binary_span_writer &ext_strm) const;

			template<typename T>
			void push(lak::tiff::tag_name id, lak::array<T> &&data)
			{
				auto &tag = tags.emplace_back();
				tag.id    = id;
				tag.set_data(lak::move(data));
			}
			template<typename T>
			void push(lak::tiff::tag_name id, const lak::span<const T> &data)
			{
				auto &tag = tags.emplace_back();
				tag.id    = id;
				tag.set_data(data);
			}

#define LAK_TIFF_TAG_MAKE(NAME, TYPE, COUNT, ...)                             \
	void push_##NAME(lak::conditional_t<(COUNT == lak::dynamic_extent) &&       \
	                                      lak::is_same_v<TYPE, char>,           \
	                                    lak::string_view<TYPE>,                 \
	                                    lak::span<const TYPE, COUNT>> data)     \
	{                                                                           \
		if constexpr (lak::is_same_v<TYPE, char>)                                 \
		{                                                                         \
			lak::array<TYPE> str;                                                   \
			str.resize(data.size());                                                \
			lak::memcpy(lak::span<byte_t>(lak::span(str)),                          \
			            lak::span<const byte_t>(lak::span<const TYPE>(data)));      \
			str.push_back(TYPE(0));                                                 \
			push<TYPE>(lak::tiff::tag_name::NAME, lak::move(str));                  \
		}                                                                         \
		else                                                                      \
			push<TYPE>(lak::tiff::tag_name::NAME, data);                            \
	}
			LAK_FOREACH_TIFF_TAG_TYPES(LAK_TIFF_TAG_MAKE)
#undef LAK_TIFF_TAG_MAKE
		};

		struct tiff
		{
			lak::tiff::image_file_header ifh;
			lak::array<lak::tiff::image_file_directory> ifd;

			template<lak::endian E>
			lak::tiff::result<> _read(lak::binary_reader &strm);

			template<lak::endian E>
			lak::tiff::result<> read(lak::binary_reader &strm);

			template<lak::endian E>
			size_t write_size() const;

			template<lak::endian E>
			requires(E == lak::endian::little || E == lak::endian::big)
			lak::tiff::result<> write(lak::binary_span_writer &strm) const;
		};
	}
}

#include "tiff.inl"

#endif
