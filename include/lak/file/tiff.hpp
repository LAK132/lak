#ifndef LAK_FILE_TIFF_HPP
#define LAK_FILE_TIFF_HPP

#include "lak/binary_reader.hpp"
#include "lak/binary_writer.hpp"
#include "lak/format.hpp"
#include "lak/math.hpp"
#include "lak/memory.hpp"
#include "lak/span.hpp"
#include "lak/stdint.hpp"
#include "lak/type_pack.hpp"
#include "lak/utility.hpp"
#include "lak/variant.hpp"

#include "lak/file/tiff_types.inl"

#include "lak/file/tiff_tags.inl"

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
		  lak::variant<lak::err::out_of_data, lak::err::value_out_of_range>>;

		using tag_types_pack =
		  lak::remove_from_pack_t<lak::bottom,
		                          lak::type_pack<
#define LAK_TIFF_TAG_TYPE(VAL, NAME, TYPE, ...) TYPE,
		                            LAK_FOREACH_TIFF_TYPE(LAK_TIFF_TAG_TYPE)
#undef LAK_TIFF_TAG_TYPE
		                              lak::bottom>>;

		template<lak::tiff::tag_name NAME>
		using tag_name_type = lak::integral_constant<lak::tiff::tag_name, NAME>;

		template<lak::tiff::tag_name TAG, typename T, size_t S>
		struct valid_tag_type_size : lak::false_type
		{
		};

#define LAK_TIFF_TAG_TYPE(NAME, TYPE, SIZE, ...)                              \
	template<>                                                                  \
	struct valid_tag_type_size<lak::tiff::tag_name::NAME, TYPE, SIZE>           \
	: lak::true_type                                                            \
	{                                                                           \
	};
		LAK_FOREACH_TIFF_TAG_TYPES(LAK_TIFF_TAG_TYPE)
#undef LAK_TIFF_TAG_TYPE

		template<lak::tiff::tag_name TAG, typename T, size_t S>
		inline constexpr bool is_valid_tag_type_size_v =
		  lak::tiff::valid_tag_type_size<TAG, T, S>::value;

		template<lak::tiff::tag_name TAG, typename T>
		struct valid_tag_type : lak::false_type
		{
		};

		template<lak::tiff::tag_name TAG, typename T>
		inline constexpr bool is_valid_tag_type_v =
		  lak::tiff::valid_tag_type<TAG, T>::value;
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
			ifd_tag(ifd_tag &&other);
			ifd_tag &operator=(ifd_tag &&other);

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

			bool visit(auto &&func) const
			{
				switch (id)
				{
#define LAK_TIFF_TAG_VISIT(VAL, NAME, ...)                                    \
	case lak::tiff::tag_name::NAME:                                             \
		return data.visit(                                                        \
		  [&]<typename T>(lak::span<T> data)                                      \
		  {                                                                       \
				if constexpr (!lak::tiff::                                            \
				                is_valid_tag_type_v<lak::tiff::tag_name::NAME, T>)    \
				{                                                                     \
					BOUNDS_ASSERT_UNREACHABLE();                                        \
					return false;                                                       \
				}                                                                     \
				else                                                                  \
				{                                                                     \
					func(lak::tiff::tag_name_type<lak::tiff::tag_name::NAME>{},         \
					     lak::span<const T>(data));                                     \
					return true;                                                        \
				}                                                                     \
		  });
					LAK_FOREACH_TIFF_TAG_VALUE(LAK_TIFF_TAG_VISIT)
#undef LAK_TIFF_TAG_VISIT
					default: break;
				}
				return false;
			}

			template<lak::endian E>
			lak::tiff::result<> read(lak::binary_reader &strm);

			// id + type + count + offset
			static constexpr size_t _write_size = 2U + 2U + 4U + 4U;
			inline size_t _data_write_size() const;
			inline size_t write_size() const;

			template<lak::endian E>
			lak::tiff::result<> write(lak::binary_span_writer &strm,
			                          lak::binary_span_writer &ext_strm) const;

#define LAK_TIFF_TAG_MAKE(NAME, TYPE, COUNT, ...)                             \
	static ifd_tag make_##NAME(lak::span<const TYPE, COUNT> data);              \
	static ifd_tag make_##NAME(lak::array<TYPE, COUNT> &&data);
			LAK_FOREACH_TIFF_TAG_TYPES(LAK_TIFF_TAG_MAKE)
#undef LAK_TIFF_TAG_MAKE
		};

		struct image_file_directory
		{
			uint32_t rows;
			lak::array<lak::tiff::strip> strips;
			// lak::array<lak::tiff::tile> tiles;
			lak::array<lak::tiff::ifd_tag> tags;
			lak::array<image_file_directory> subifds;
			lak::unique_ptr<image_file_directory> exif;
			lak::unique_ptr<image_file_directory> interop;
			lak::unique_ptr<image_file_directory> kodak;
			uint32_t _ifd_offset;

			inline size_t total_tag_count() const
			{
				return tags.size() + (strips.empty() ? 0U : 3U) +
				       (subifds.empty() ? 0U : 1U) + (exif ? 1U : 0U);
			}

			template<lak::endian E>
			lak::tiff::result<> read(lak::binary_reader &strm);

			inline size_t _write_size() const;
			size_t write_size() const;

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
			inline void push(lak::tiff::tag_name id, lak::astring_view data)
			{
				lak::array<char> str;
				str.resize(data.size());
				lak::memcpy(lak::span<byte_t>(lak::span(str)),
				            lak::span<const byte_t>(lak::span<const char>(data)));
				str.push_back('\0');
				push<char>(id, lak::move(str));
			}

			inline lak::tiff::strip &push_strip() { return strips.emplace_back(); }

			// inline lak::tiff::tile &push_tile() { return tiles.emplace_back(); }

			inline image_file_directory &push_subifd()
			{
				return subifds.emplace_back();
			}

			inline image_file_directory &push_exif()
			{
				ASSERT(!exif);
				return *(exif = decltype(exif)::make());
			}

			inline image_file_directory &push_interop()
			{
				ASSERT(!interop);
				return *(interop = decltype(interop)::make());
			}

			inline image_file_directory &push_kodak()
			{
				ASSERT(!kodak);
				return *(kodak = decltype(kodak)::make());
			}

#define LAK_TIFF_TAG_MAKE(NAME, TYPE, COUNT, ...)                             \
	void push_##NAME(lak::span<const TYPE, COUNT> data);
			LAK_FOREACH_TIFF_TAG_TYPES(LAK_TIFF_TAG_MAKE)
#undef LAK_TIFF_TAG_MAKE

#define LAK_TIFF_TAG_MAKE(NAME, ...) void push_##NAME(lak::astring_view data);
			LAK_FOREACH_STRING_TIFF_TAG(LAK_TIFF_TAG_MAKE)
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

#include "lak/file/tiff.inl"

#endif
