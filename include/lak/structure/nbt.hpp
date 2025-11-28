#ifndef LAK_STRUCTURE_NBT_HPP
#define LAK_STRUCTURE_NBT_HPP

#include "lak/binary_reader.hpp"
#include "lak/binary_writer.hpp"
#include "lak/span.hpp"
#include "lak/stdint.hpp"
#include "lak/type_pack.hpp"
#include "lak/utility.hpp"
#include "lak/variant.hpp"

#include "lak/string_literals/string.hpp"

namespace lak
{
	namespace nbt
	{
		struct named_tag;
		struct TAG_List;

		struct invalid_type_error
		{
		};
	}
}

template<>
struct lak::_array_type_is_copyable<lak::nbt::named_tag> : lak::true_type
{
};

template<>
struct lak::_array_type_is_copyable<lak::nbt::TAG_List> : lak::true_type
{
};

namespace lak
{
#define LAK_FOREACH_NBT_TYPE(MACRO, ...)                                      \
	MACRO(0, End, TAG_End, __VA_ARGS__)                                         \
	MACRO(1, Byte, TAG_Byte, __VA_ARGS__)                                       \
	MACRO(2, Short, TAG_Short, __VA_ARGS__)                                     \
	MACRO(3, Int, TAG_Int, __VA_ARGS__)                                         \
	MACRO(4, Long, TAG_Long, __VA_ARGS__)                                       \
	MACRO(5, Float, TAG_Float, __VA_ARGS__)                                     \
	MACRO(6, Double, TAG_Double, __VA_ARGS__)                                   \
	MACRO(7, Byte_Array, TAG_Byte_Array, __VA_ARGS__)                           \
	MACRO(8, String, TAG_String, __VA_ARGS__)                                   \
	MACRO(9, List, TAG_List, __VA_ARGS__)                                       \
	MACRO(10, Compound, TAG_Compound, __VA_ARGS__)                              \
	MACRO(11, Int_Array, TAG_Int_Array, __VA_ARGS__)                            \
	MACRO(12, Long_Array, TAG_Long_Array, __VA_ARGS__)

	namespace nbt
	{
		enum struct tag_type : uint8_t
		{
#define LAK_NBT_ENUM(VAL, NAME, ...) NAME = VAL,
			LAK_FOREACH_NBT_TYPE(LAK_NBT_ENUM)
#undef LAK_NBT_ENUM
		};
	}
}

LAK_MEMCPY_BYTE_TRAITS_IMPL(lak::nbt::tag_type)

namespace lak
{
	namespace nbt
	{
		template<typename T>
		struct pod_tag;

		using TAG_Byte   = lak::nbt::pod_tag<int8_t>;
		using TAG_Short  = lak::nbt::pod_tag<int16_t>;
		using TAG_Int    = lak::nbt::pod_tag<int32_t>;
		using TAG_Long   = lak::nbt::pod_tag<int64_t>;
		using TAG_Float  = lak::nbt::pod_tag<f32_t>;
		using TAG_Double = lak::nbt::pod_tag<f64_t>;

		template<typename T>
		struct pod_tag
		{
			using value_type = T;
			value_type value;

			inline friend std::ostream &operator<<(std::ostream &strm,
			                                       const lak::nbt::pod_tag<T> &tag)
			{
				if constexpr (lak::is_same_v<T, lak::nbt::TAG_Byte::value_type>)
					return strm << std::dec << intmax_t(tag.value) << "b";
				else if constexpr (lak::is_same_v<T, lak::nbt::TAG_Short::value_type>)
					return strm << std::dec << intmax_t(tag.value) << "s";
				else if constexpr (lak::is_same_v<T, lak::nbt::TAG_Int::value_type>)
					return strm << std::dec << intmax_t(tag.value);
				else if constexpr (lak::is_same_v<T, lak::nbt::TAG_Long::value_type>)
					return strm << std::dec << intmax_t(tag.value) << "l";
				else if constexpr (lak::is_same_v<T, lak::nbt::TAG_Float::value_type>)
					return strm << std::dec << tag.value << "f";
				else if constexpr (lak::is_same_v<T, lak::nbt::TAG_Double::value_type>)
					return strm << std::dec << tag.value << "d";
			}
		};
	}
}

LAK_FIXED_TEMPLATE_STRUCT_BYTES_TRAITS(typename T,
                                       lak::nbt::pod_tag<T>,
                                       &lak::nbt::pod_tag<T>::value)

namespace lak
{
	namespace nbt
	{
		template<typename T>
		struct array_tag;

		using TAG_Byte_Array = lak::nbt::array_tag<TAG_Byte::value_type>;
		using TAG_Int_Array  = lak::nbt::array_tag<TAG_Int::value_type>;
		using TAG_Long_Array = lak::nbt::array_tag<TAG_Long::value_type>;

		template<typename T>
		struct array_tag
		{
			using value_type = T;
			lak::array<value_type> value;

			template<lak::endian E>
			lak::error_code<lak::err::out_of_data> read(lak::binary_reader &strm)
			{
				RES_TRY_ASSIGN(auto size =,
				               strm.template read<lak::nbt::TAG_Int, E>());
				RES_TRY_ASSIGN(value =, strm.template read<value_type, E>(size.value));
				return lak::ok_t{};
			}

			template<lak::endian E>
			size_t write_size() const
			{
				size_t result = lak::to_bytes_traits<lak::nbt::TAG_Int, E>::size;
				if constexpr (lak::to_bytes_traits<T, E>::const_size)
					result += (value.size() * lak::to_bytes_traits<T, E>::size);
				else
					for (const auto &v : value)
						result += lak::to_bytes_traits<T, E>::dynamic_size(v);
				return result;
			}

			template<lak::endian E>
			lak::error_code<lak::err::out_of_data> write(
			  lak::binary_span_writer &strm) const
			{
				RES_TRY(strm.template write<E>(lak::nbt::TAG_Int{
				  .value = static_cast<lak::nbt::TAG_Int::value_type>(value.size()),
				}));
				RES_TRY(strm.template write<E>(lak::span(value)));
				return lak::ok_t{};
			}

			inline friend std::ostream &operator<<(std::ostream &strm,
			                                       const lak::nbt::array_tag<T> &tag)
			{
				if constexpr (lak::is_same_v<T, lak::nbt::TAG_Byte::value_type>)
					strm << "[B;";
				else if constexpr (lak::is_same_v<T, lak::nbt::TAG_Int::value_type>)
					strm << "[I;";
				else if constexpr (lak::is_same_v<T, lak::nbt::TAG_Long::value_type>)
					strm << "[L;";

				return strm << lak::as_astring(lak::accumulate(
				                 lak::span(tag.value),
				                 u8""_str,
				                 [](const lak::u8string &str, T val)
				                 {
					                 lak::nbt::pod_tag<T> v{.value = val};
					                 return str.empty()
					                          ? lak::streamify(v)
					                          : lak::spaced_streamify(u8","_str, str, v);
				                 }))
				            << "]";
			}
		};

		struct TAG_End
		{
			lak::array<byte_t, 0> _value;

			inline friend std::ostream &operator<<(std::ostream &strm,
			                                       const lak::nbt::TAG_End &)
			{
				return strm;
			}
		};
	}
}

LAK_FIXED_STRUCT_BYTES_TRAITS(lak::nbt::TAG_End, &lak::nbt::TAG_End::_value)

namespace lak
{
	namespace nbt
	{
		struct TAG_String
		{
			lak::u8string value;

			template<lak::endian E>
			lak::error_code<lak::err::out_of_data> read(lak::binary_reader &strm)
			{
				RES_TRY_ASSIGN(auto size =,
				               strm.template read<lak::nbt::TAG_Short, E>());
				RES_TRY_ASSIGN(value =,
				               strm.template read_exact_c_str<char8_t, E>(size.value));
				return lak::ok_t{};
			}

			template<lak::endian E>
			size_t write_size() const
			{
				return lak::to_bytes_traits<lak::nbt::TAG_Short, E>::size +
				       value.size();
			}

			template<lak::endian E>
			lak::error_code<lak::err::out_of_data> write(
			  lak::binary_span_writer &strm) const
			{
				RES_TRY(strm.template write<E>(lak::nbt::TAG_Short{
				  .value = static_cast<lak::nbt::TAG_Short::value_type>(value.size()),
				}));
				RES_TRY(strm.template write<E>(lak::span(value)));
				return lak::ok_t{};
			}

			inline friend std::ostream &operator<<(std::ostream &strm,
			                                       const lak::nbt::TAG_String &tag)
			{
				return strm << "\'" << lak::as_astring(tag.value) << "\'";
			}
		};

		struct TAG_Compound
		{
			using value_type = lak::array<lak::nbt::named_tag>;
			value_type value;

			template<lak::endian E>
			lak::error_code<lak::err::out_of_data> read(lak::binary_reader &strm)
			{
				for (;;)
				{
					if (auto res = strm.template peek<lak::nbt::tag_type, E>();
					    res.is_ok())
					{
						auto &t = res.unsafe_unwrap();
						if (t == lak::nbt::tag_type::End)
						{
							strm.template read<lak::nbt::tag_type, E>().unwrap();
							break;
						}
					}
					else
						return lak::move_err(res.unsafe_unwrap_err());

					RES_TRY_ASSIGN(auto tag =,
					               strm.template read<lak::nbt::named_tag, E>());
					value.push_back(lak::move(tag));
				}
				return lak::ok_t{};
			}

			template<lak::endian E>
			size_t write_size() const;

			template<lak::endian E>
			lak::error_code<lak::err::out_of_data> write(
			  lak::binary_span_writer &strm) const
			{
				RES_TRY(strm.template write<E>(lak::span(value)));
				RES_TRY(strm.template write<E>(lak::nbt::tag_type::End));
				return lak::ok_t{};
			}

			inline friend std::ostream &operator<<(std::ostream &strm,
			                                       const lak::nbt::TAG_Compound &tag)
			{
				return strm << "{"
				            << lak::as_astring(lak::accumulate(
				                 lak::span(tag.value),
				                 u8""_str,
				                 [](const lak::u8string &str,
				                    const lak::nbt::named_tag &val)
				                 {
					                 return str.empty() ? lak::streamify(val)
					                                    : lak::spaced_streamify(
					                                        u8","_str, str, val);
				                 }))
				            << "}";
			}
		};

		using tag_types_pack =
		  lak::remove_from_pack_t<lak::bottom,
		                          lak::type_pack<
#define LAK_NBT_PAYLOAD(x, y, NAME, ...) NAME,
		                            LAK_FOREACH_NBT_TYPE(LAK_NBT_PAYLOAD)
#undef LAK_NBT_PAYLOAD
		                              lak::bottom>>;
	}
}

template<>
struct lak::_array_type_is_copyable<lak::nbt::tag_types_pack> : lak::true_type
{
};

namespace lak
{
	namespace nbt
	{
		struct TAG_List
		{
			// use vector instead of array because it only has one template parameter
			using value_type = lak::create_from_pack_t<
			  lak::variant,
			  lak::pack_foreach_t<lak::vector, lak::nbt::tag_types_pack>>;
			value_type value;

			inline lak::nbt::tag_type type() const
			{
				return static_cast<lak::nbt::tag_type>(value.index());
			}

			inline size_t size() const
			{
				return value.visit([](const auto &arr) { return arr.size(); });
			}

			template<lak::endian E>
			lak::error_codes<lak::err::out_of_data, lak::nbt::invalid_type_error>
			read(lak::binary_reader &strm)
			{
				RES_TRY_ASSIGN(auto type =,
				               strm.template read<lak::nbt::tag_type, E>());
				RES_TRY_ASSIGN(auto size =,
				               strm.template read<lak::nbt::TAG_Int, E>());
				switch (type)
				{
#define LAK_NBT_READER_VISIT(VAL, NAME, TAG, ...)                             \
	case lak::nbt::tag_type::NAME:                                              \
	{                                                                           \
		RES_TRY_ASSIGN(value =,                                                   \
		               strm.template read<lak::nbt::TAG, E>(size.value));         \
	}                                                                           \
	break;
					LAK_FOREACH_NBT_TYPE(LAK_NBT_READER_VISIT)
#undef LAK_NBT_READER_VISIT
					default:
						return lak::err_t<lak::nbt::invalid_type_error>{};
				}

				return lak::ok_t{};
			}

			template<lak::endian E>
			size_t write_size() const
			{
				size_t result = lak::to_bytes_traits<lak::nbt::tag_type, E>::size +
				                lak::to_bytes_traits<lak::nbt::TAG_Int, E>::size;
				value.visit(
				  [&]<typename T>(const lak::array<T> &arr)
				  {
					  for (const auto &a : arr)
						  result += lak::to_bytes_traits<T, E>::dynamic_size(a);
				  });
				return result;
			}

			template<lak::endian E>
			lak::error_code<lak::err::out_of_data> write(
			  lak::binary_span_writer &strm) const
			{
				RES_TRY(strm.template write<E>(type()));
				RES_TRY(strm.template write<E>(lak::nbt::TAG_Int{
				  .value = static_cast<lak::nbt::TAG_Int::value_type>(size()),
				}));
				RES_TRY(value.visit(
				  [&]<typename T>(
				    const lak::array<T> &arr) -> lak::error_code<lak::err::out_of_data>
				  { return strm.template write<E>(lak::span(arr)); }));
				return lak::ok_t{};
			}

			inline friend std::ostream &operator<<(std::ostream &strm,
			                                       const lak::nbt::TAG_List &tag)
			{
				return strm << "["
				            << lak::as_astring(tag.value.visit(
				                 []<typename T>(const lak::array<T> &arr)
				                 {
					                 return lak::accumulate(
					                   lak::span(arr),
					                   u8""_str,
					                   [](const lak::u8string &str, const T &val)
					                   {
						                   return str.empty() ? lak::streamify(val)
						                                      : lak::spaced_streamify(
						                                          u8","_str, str, val);
					                   });
				                 }))
				            << "]";
			}
		};

		struct tag_payload
		{
			using value_type =
			  lak::create_from_pack_t<lak::variant, lak::nbt::tag_types_pack>;

			value_type value;

			inline lak::nbt::tag_type type() const
			{
				return static_cast<lak::nbt::tag_type>(value.index());
			}

			template<lak::endian E>
			size_t write_size() const
			{
				return value.visit(
				  []<typename T>(const T &val)
				  { return lak::to_bytes_traits<T, E>::dynamic_size(val); });
			}

			template<lak::endian E>
			lak::error_code<lak::err::out_of_data> write(
			  lak::binary_span_writer &strm) const
			{
				return value.visit([&]<typename T>(const T &val)
				                   { return strm.template write<E>(val); });
			}

			inline friend std::ostream &operator<<(
			  std::ostream &strm, const lak::nbt::tag_payload &payload)
			{
				payload.value.visit([&]<typename T>(const T &val) { strm << val; });
				return strm;
			}
		};

		struct named_tag
		{
			lak::nbt::TAG_String name;
			lak::nbt::tag_payload payload;
			inline lak::nbt::tag_type type() const { return payload.type(); }

			template<lak::endian E>
			lak::error_codes<lak::err::out_of_data, lak::nbt::invalid_type_error>
			read(lak::binary_reader &strm)
			{
				RES_TRY_ASSIGN(lak::nbt::tag_type type =,
				               strm.template read<lak::nbt::tag_type, E>());
				RES_TRY(name.template read<E>(strm));
				switch (type)
				{
#define LAK_NBT_READER_VISIT(VAL, NAME, TAG, ...)                             \
	case lak::nbt::tag_type::NAME:                                              \
	{                                                                           \
		RES_TRY_ASSIGN(payload.value =, strm.template read<lak::nbt::TAG, E>());  \
	}                                                                           \
	break;
					LAK_FOREACH_NBT_TYPE(LAK_NBT_READER_VISIT)
#undef LAK_NBT_READER_VISIT
					default:
						return lak::err_t{};
				}
				return lak::ok_t{};
			}

			template<lak::endian E>
			size_t write_size() const
			{
				return lak::to_bytes_traits<lak::nbt::tag_type, E>::size +
				       name.template write_size<E>() +
				       payload.template write_size<E>();
			}

			template<lak::endian E>
			lak::error_code<lak::err::out_of_data> write(
			  lak::binary_span_writer &strm) const
			{
				RES_TRY(strm.template write<E>(type()));
				RES_TRY(name.template write<E>(strm));
				RES_TRY(payload.template write<E>(strm));
				return lak::ok_t{};
			}

			inline friend std::ostream &operator<<(std::ostream &strm,
			                                       const lak::nbt::named_tag &tag)
			{
				return strm << lak::as_astring(tag.name.value) << ":" << tag.payload;
			}
		};

		static_assert(!lak::is_same_v<
		              typename lak::to_bytes_traits<lak::nbt::named_tag,
		                                            lak::endian::big>::error_type,
		              lak::nonesuch>);
		static_assert(
		  !lak::is_same_v<
		    typename lak::from_bytes_traits<lak::nbt::named_tag,
		                                    lak::endian::big>::error_type,
		    lak::nonesuch>);
		static_assert(lak::concepts::to_bytes_writeable<lak::nbt::named_tag,
		                                                lak::endian::native>);
		static_assert(lak::concepts::from_bytes_readable<lak::nbt::named_tag,
		                                                 lak::endian::native>);

		template<lak::endian E>
		size_t TAG_Compound::write_size() const
		{
			size_t result = lak::to_bytes_traits<lak::nbt::tag_type, E>::size;
			for (const auto &v : value)
				result +=
				  lak::to_bytes_traits<lak::nbt::named_tag, E>::dynamic_size(v);
			return result;
		}

		inline named_tag make_byte(lak::u8string name,
		                           lak::nbt::TAG_Byte::value_type value)
		{
			return {
			  .name    = {.value = lak::move(name)},
			  .payload = {.value = TAG_Byte{.value = value}},
			};
		}

		inline named_tag make_short(lak::u8string name,
		                            lak::nbt::TAG_Short::value_type value)
		{
			return {
			  .name    = {.value = lak::move(name)},
			  .payload = {.value = TAG_Short{.value = value}},
			};
		}

		inline named_tag make_int(lak::u8string name,
		                          lak::nbt::TAG_Int::value_type value)
		{
			return {
			  .name    = {.value = lak::move(name)},
			  .payload = {.value = TAG_Int{.value = value}},
			};
		}

		inline named_tag make_long(lak::u8string name,
		                           lak::nbt::TAG_Long::value_type value)
		{
			return {
			  .name    = {.value = lak::move(name)},
			  .payload = {.value = TAG_Long{.value = value}},
			};
		}

		inline named_tag make_float(lak::u8string name,
		                            lak::nbt::TAG_Float::value_type value)
		{
			return {
			  .name    = {.value = lak::move(name)},
			  .payload = {.value = TAG_Float{.value = value}},
			};
		}

		inline named_tag make_double(lak::u8string name,
		                             lak::nbt::TAG_Double::value_type value)
		{
			return {
			  .name    = {.value = lak::move(name)},
			  .payload = {.value = TAG_Double{.value = value}},
			};
		}

		inline named_tag make_byte_array(
		  lak::u8string name,
		  lak::array<lak::nbt::TAG_Byte_Array::value_type> value)
		{
			return {
			  .name    = {.value = lak::move(name)},
			  .payload = {.value = TAG_Byte_Array{.value = lak::move(value)}},
			};
		}

		inline named_tag make_string(lak::u8string name, lak::u8string value)
		{
			return {
			  .name    = {.value = lak::move(name)},
			  .payload = {.value = TAG_String{.value = lak::move(value)}},
			};
		}

		template<typename T>
		inline named_tag make_list(lak::u8string name, lak::array<T> value)
		{
			return {
			  .name    = {.value = lak::move(name)},
			  .payload = {.value = TAG_List{.value = lak::move(value)}},
			};
		}

		inline named_tag make_compound(lak::u8string name,
		                               lak::array<lak::nbt::named_tag> value)
		{
			return {
			  .name    = {.value = lak::move(name)},
			  .payload = {.value = TAG_Compound{.value = lak::move(value)}},
			};
		}

		inline named_tag make_int_array(
		  lak::u8string name,
		  lak::array<lak::nbt::TAG_Int_Array::value_type> value)
		{
			return {
			  .name    = {.value = lak::move(name)},
			  .payload = {.value = TAG_Int_Array{.value = lak::move(value)}},
			};
		}

		inline named_tag make_long_array(
		  lak::u8string name,
		  lak::array<lak::nbt::TAG_Long_Array::value_type> value)
		{
			return {
			  .name    = {.value = lak::move(name)},
			  .payload = {.value = TAG_Long_Array{.value = lak::move(value)}},
			};
		}
	}
}

static_assert(
  lak::concepts::to_bytes_writeable<lak::nbt::tag_type, lak::endian::little>);
static_assert(
  lak::concepts::to_bytes_writeable<lak::nbt::named_tag, lak::endian::little>);
static_assert(
  lak::concepts::to_bytes_writeable<lak::nbt::TAG_End, lak::endian::little>);
static_assert(
  lak::concepts::to_bytes_writeable<lak::nbt::TAG_Byte, lak::endian::little>);
static_assert(
  lak::concepts::to_bytes_writeable<lak::nbt::TAG_Short, lak::endian::little>);
static_assert(
  lak::concepts::to_bytes_writeable<lak::nbt::TAG_Int, lak::endian::little>);
static_assert(
  lak::concepts::to_bytes_writeable<lak::nbt::TAG_Long, lak::endian::little>);
static_assert(
  lak::concepts::to_bytes_writeable<lak::nbt::TAG_Float, lak::endian::little>);
static_assert(lak::concepts::to_bytes_writeable<lak::nbt::TAG_Double,
                                                lak::endian::little>);
static_assert(lak::concepts::to_bytes_writeable<lak::nbt::TAG_Byte_Array,
                                                lak::endian::little>);
static_assert(lak::concepts::to_bytes_writeable<lak::nbt::TAG_String,
                                                lak::endian::little>);
static_assert(
  lak::concepts::to_bytes_writeable<lak::nbt::TAG_List, lak::endian::little>);
static_assert(lak::concepts::to_bytes_writeable<lak::nbt::TAG_Compound,
                                                lak::endian::little>);
static_assert(lak::concepts::to_bytes_writeable<lak::nbt::TAG_Int_Array,
                                                lak::endian::little>);
static_assert(lak::concepts::to_bytes_writeable<lak::nbt::TAG_Long_Array,
                                                lak::endian::little>);

static_assert(
  lak::concepts::from_bytes_readable<lak::nbt::tag_type, lak::endian::little>);
static_assert(lak::concepts::from_bytes_readable<lak::nbt::named_tag,
                                                 lak::endian::little>);
static_assert(
  lak::concepts::from_bytes_readable<lak::nbt::TAG_End, lak::endian::little>);
static_assert(
  lak::concepts::from_bytes_readable<lak::nbt::TAG_Byte, lak::endian::little>);
static_assert(lak::concepts::from_bytes_readable<lak::nbt::TAG_Short,
                                                 lak::endian::little>);
static_assert(
  lak::concepts::from_bytes_readable<lak::nbt::TAG_Int, lak::endian::little>);
static_assert(
  lak::concepts::from_bytes_readable<lak::nbt::TAG_Long, lak::endian::little>);
static_assert(lak::concepts::from_bytes_readable<lak::nbt::TAG_Float,
                                                 lak::endian::little>);
static_assert(lak::concepts::from_bytes_readable<lak::nbt::TAG_Double,
                                                 lak::endian::little>);
static_assert(lak::concepts::from_bytes_readable<lak::nbt::TAG_Byte_Array,
                                                 lak::endian::little>);
static_assert(lak::concepts::from_bytes_readable<lak::nbt::TAG_String,
                                                 lak::endian::little>);
static_assert(
  lak::concepts::from_bytes_readable<lak::nbt::TAG_List, lak::endian::little>);
static_assert(lak::concepts::from_bytes_readable<lak::nbt::TAG_Compound,
                                                 lak::endian::little>);
static_assert(lak::concepts::from_bytes_readable<lak::nbt::TAG_Int_Array,
                                                 lak::endian::little>);
static_assert(lak::concepts::from_bytes_readable<lak::nbt::TAG_Long_Array,
                                                 lak::endian::little>);

#endif
