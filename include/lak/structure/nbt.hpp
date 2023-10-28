#ifndef LAK_STRUCTURE_NBT_HPP
#define LAK_STRUCTURE_NBT_HPP

#include "lak/binary_reader.hpp"
#include "lak/binary_writer.hpp"
#include "lak/span.hpp"
#include "lak/stdint.hpp"
#include "lak/type_pack.hpp"
#include "lak/utility.hpp"
#include "lak/variant.hpp"

namespace lak
{
	namespace nbt
	{
		struct named_tag;
		struct TAG_List;
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
			inline friend std::ostream &operator<<(std::ostream &strm,
			                                       const lak::nbt::TAG_End &)
			{
				return strm;
			}
		};

		struct TAG_String
		{
			lak::u8string value;

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

			inline friend std::ostream &operator<<(std::ostream &strm,
			                                       const lak::nbt::named_tag &tag)
			{
				return strm << lak::as_astring(tag.name.value) << ":" << tag.payload;
			}
		};

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

template<lak::endian E>
struct lak::to_bytes_traits<lak::nbt::tag_type, E>
: lak::to_bytes_traits_arr_impl<lak::nbt::tag_type, E>
{
	static constexpr bool const_size = true;
	static constexpr size_t size() { return 1; }

	static auto single_to_bytes(const lak::nbt::tag_type &val)
	{
		return lak::to_bytes<uint8_t, lak::endian::big>(static_cast<uint8_t>(val));
	}
};

template<lak::endian E>
struct lak::to_bytes_traits<lak::nbt::named_tag, E>
: lak::to_bytes_traits_arr_impl<lak::nbt::named_tag, E>
{
	static constexpr bool const_size = false;
	static size_t size(const lak::nbt::named_tag &tag)
	{
		return lak::to_bytes_size<lak::nbt::tag_type, E>() +
		       lak::to_bytes_size<lak::nbt::TAG_String, E>(tag.name) +
		       lak::to_bytes_size<lak::nbt::tag_payload, E>(tag.payload);
	}

	static void single_to_bytes(lak::span<byte_t> dst,
	                            const lak::nbt::named_tag &src)
	{
		lak::binary_span_writer strm(dst);
		strm.template write<E>(src.type()).unwrap();
		strm.template write<E>(src.name).unwrap();
		strm.template write<E>(src.payload).unwrap();
	}
};

template<typename T, lak::endian E>
struct lak::to_bytes_traits<lak::nbt::pod_tag<T>, E>
: lak::to_bytes_traits_arr_impl<lak::nbt::pod_tag<T>, E>
{
	static constexpr bool const_size = true;
	static constexpr size_t size() { return sizeof(T); }

	static auto single_to_bytes(const lak::nbt::pod_tag<T> &val)
	{
		return lak::to_bytes<T, lak::endian::big>(val.value);
	}
};

template<typename T, lak::endian E>
struct lak::to_bytes_traits<lak::nbt::array_tag<T>, E>
: lak::to_bytes_traits_arr_impl<lak::nbt::array_tag<T>, E>
{
	static constexpr bool const_size = false;
	static size_t size(const lak::nbt::array_tag<T> &arr)
	{
		return lak::to_bytes_traits<lak::nbt::TAG_Int, E>::size() +
		       (arr.value.size() * sizeof(T));
	}

	static void single_to_bytes(lak::span<byte_t> dst,
	                            const lak::nbt::array_tag<T> &src)
	{
		lak::binary_span_writer strm(dst);
		strm
		  .template write<E>(lak::nbt::TAG_Int{
		    .value = static_cast<lak::nbt::TAG_Int::value_type>(src.value.size())})
		  .unwrap();
		strm.template write<lak::endian::big>(lak::span(src.value)).unwrap();
	}
};

template<lak::endian E>
struct lak::to_bytes_traits<lak::nbt::TAG_End, E>
: lak::to_bytes_traits_arr_impl<lak::nbt::TAG_End, E>
{
	static constexpr bool const_size = true;
	static constexpr size_t size() { return 0; }

	static auto single_to_bytes(const lak::nbt::TAG_End &val)
	{
		return lak::array<byte_t, 0>{};
	}
};

template<lak::endian E>
struct lak::to_bytes_traits<lak::nbt::TAG_String, E>
: lak::to_bytes_traits_arr_impl<lak::nbt::TAG_String, E>
{
	static constexpr bool const_size = false;
	static size_t size(const lak::nbt::TAG_String &arr)
	{
		return lak::to_bytes_traits<lak::nbt::TAG_Short, E>::size() +
		       arr.value.size();
	}

	static void single_to_bytes(lak::span<byte_t> dst,
	                            const lak::nbt::TAG_String &src)
	{
		lak::binary_span_writer strm(dst);
		strm
		  .template write<E>(lak::nbt::TAG_Short{
		    .value =
		      static_cast<lak::nbt::TAG_Short::value_type>(src.value.size())})
		  .unwrap();
		strm.template write<E>(lak::span(src.value)).unwrap();
	}
};

template<lak::endian E>
struct lak::to_bytes_traits<lak::nbt::TAG_List, E>
: lak::to_bytes_traits_arr_impl<lak::nbt::TAG_List, E>
{
	static constexpr bool const_size = false;
	static size_t size(const lak::nbt::TAG_List &list)
	{
		return lak::to_bytes_size<lak::nbt::tag_type, E>() +
		       lak::to_bytes_size<lak::nbt::TAG_Int, E>() +
		       list.value.visit(
		         []<typename T>(const lak::array<T> &arr)
		         { return lak::to_bytes_size<T, E>(lak::span(arr)); });
	}

	static void single_to_bytes(lak::span<byte_t> dst,
	                            const lak::nbt::TAG_List &src)
	{
		lak::binary_span_writer strm{dst};
		strm.template write<E>(src.type()).unwrap();
		strm
		  .template write<E>(lak::nbt::TAG_Int{
		    .value = static_cast<lak::nbt::TAG_Int::value_type>(src.size())})
		  .unwrap();
		src.value.visit([&]<typename T>(const lak::array<T> &arr)
		                { strm.template write<E>(lak::span(arr)).unwrap(); });
	}
};

template<lak::endian E>
struct lak::to_bytes_traits<lak::nbt::TAG_Compound, E>
: lak::to_bytes_traits_arr_impl<lak::nbt::TAG_Compound, E>
{
	static constexpr bool const_size = false;
	static size_t size(const lak::nbt::TAG_Compound &compound)
	{
		return lak::to_bytes_size<lak::nbt::named_tag, E>(
		         lak::span(compound.value)) +
		       lak::to_bytes_size<lak::nbt::tag_type, E>();
	}

	static void single_to_bytes(lak::span<byte_t> dst,
	                            const lak::nbt::TAG_Compound &src)
	{
		lak::binary_span_writer strm(dst);
		strm.template write<E>(lak::span(src.value)).unwrap();
		strm.template write<E>(lak::nbt::tag_type::End).unwrap();
	}
};

template<lak::endian E>
struct lak::to_bytes_traits<lak::nbt::tag_payload, E>
: lak::to_bytes_traits_arr_impl<lak::nbt::tag_payload, E>
{
	static constexpr bool const_size = false;
	static size_t size(const lak::nbt::tag_payload &payload)
	{
		return payload.value.visit([]<typename T>(const T &val)
		                           { return lak::to_bytes_size<T, E>(val); });
	}

	static void single_to_bytes(lak::span<byte_t> dst,
	                            const lak::nbt::tag_payload &src)
	{
		src.value.visit([&]<typename T>(const T &val)
		                { return lak::to_bytes<T, E>(dst, val).unwrap(); });
	}
};

static_assert(
  lak::has_to_bytes_traits<lak::nbt::tag_type, lak::endian::little>);
static_assert(
  lak::has_to_bytes_traits<lak::nbt::named_tag, lak::endian::little>);
static_assert(
  lak::has_to_bytes_traits<lak::nbt::TAG_End, lak::endian::little>);
static_assert(
  lak::has_to_bytes_traits<lak::nbt::TAG_Byte, lak::endian::little>);
static_assert(
  lak::has_to_bytes_traits<lak::nbt::TAG_Short, lak::endian::little>);
static_assert(
  lak::has_to_bytes_traits<lak::nbt::TAG_Int, lak::endian::little>);
static_assert(
  lak::has_to_bytes_traits<lak::nbt::TAG_Long, lak::endian::little>);
static_assert(
  lak::has_to_bytes_traits<lak::nbt::TAG_Float, lak::endian::little>);
static_assert(
  lak::has_to_bytes_traits<lak::nbt::TAG_Double, lak::endian::little>);
static_assert(
  lak::has_to_bytes_traits<lak::nbt::TAG_Byte_Array, lak::endian::little>);
static_assert(
  lak::has_to_bytes_traits<lak::nbt::TAG_String, lak::endian::little>);
static_assert(
  lak::has_to_bytes_traits<lak::nbt::TAG_List, lak::endian::little>);
static_assert(
  lak::has_to_bytes_traits<lak::nbt::TAG_Compound, lak::endian::little>);
static_assert(
  lak::has_to_bytes_traits<lak::nbt::TAG_Int_Array, lak::endian::little>);
static_assert(
  lak::has_to_bytes_traits<lak::nbt::TAG_Long_Array, lak::endian::little>);
static_assert(
  lak::has_to_bytes_traits<lak::nbt::tag_payload, lak::endian::little>);

template<lak::endian E>
struct lak::from_bytes_traits<lak::nbt::tag_type, E>
{
	using value_type                 = lak::nbt::tag_type;
	static constexpr bool const_size = true;
	static constexpr size_t size     = 1U;

	static void from_bytes(lak::from_bytes_data<value_type, E> data)
	{
		lak::binary_reader strm{data.src};
		for (auto &v : data.dst)
			v = static_cast<lak::nbt::tag_type>(strm.read_u8().unwrap());
	}
};

template<lak::endian E>
struct lak::from_bytes_traits<lak::nbt::named_tag, E>
{
	using value_type                 = lak::nbt::named_tag;
	static constexpr bool const_size = false;
	static constexpr size_t size     = lak::dynamic_extent;

	static lak::result<lak::span<const byte_t>> from_bytes(
	  lak::from_bytes_data<value_type, E> data)
	{
		lak::binary_reader strm{data.src};
		for (auto &v : data.dst)
		{
			RES_TRY_ASSIGN(lak::nbt::tag_type type =,
			               strm.template read<lak::nbt::tag_type>());
			RES_TRY_ASSIGN(v.name =, strm.template read<lak::nbt::TAG_String>());
			switch (type)
			{
#define LAK_NBT_READER_VISIT(VAL, NAME, TAG, ...)                             \
	case lak::nbt::tag_type::NAME:                                              \
	{                                                                           \
		RES_TRY_ASSIGN(v.payload.value =, strm.template read<lak::nbt::TAG>());   \
	}                                                                           \
	break;
				LAK_FOREACH_NBT_TYPE(LAK_NBT_READER_VISIT)
#undef LAK_NBT_READER_VISIT
				default:
					return lak::err_t{};
			}
		}

		return lak::ok_t{strm.remaining()};
	}
};

template<typename T, lak::endian E>
struct lak::from_bytes_traits<lak::nbt::pod_tag<T>, E>
{
	using value_type                 = lak::nbt::pod_tag<T>;
	static constexpr bool const_size = true;
	static constexpr size_t size     = sizeof(T);

	static void from_bytes(lak::from_bytes_data<value_type, E> data)
	{
		lak::binary_reader strm{data.src};
		for (auto &v : data.dst)
			v.value = strm.template read<T, lak::endian::big>().unwrap();
	}
};

template<typename T, lak::endian E>
struct lak::from_bytes_traits<lak::nbt::array_tag<T>, E>
{
	using value_type                 = lak::nbt::array_tag<T>;
	static constexpr bool const_size = false;
	static constexpr size_t size     = lak::dynamic_extent;

	static lak::result<lak::span<const byte_t>> from_bytes(
	  lak::from_bytes_data<value_type, E> data)
	{
		lak::binary_reader strm{data.src};
		for (auto &v : data.dst)
		{
			RES_TRY_ASSIGN(auto size =, strm.template read<lak::nbt::TAG_Int>());
			RES_TRY_ASSIGN(v.value =,
			               strm.template read<T, lak::endian::big>(size.value));
		}
		return lak::ok_t{strm.remaining()};
	}
};

template<lak::endian E>
struct lak::from_bytes_traits<lak::nbt::TAG_End, E>
{
	using value_type                 = lak::nbt::TAG_End;
	static constexpr bool const_size = true;
	static constexpr size_t size     = 0U;

	static void from_bytes(lak::from_bytes_data<value_type, E> data) {}
};

template<lak::endian E>
struct lak::from_bytes_traits<lak::nbt::TAG_String, E>
{
	using value_type                 = lak::nbt::TAG_String;
	static constexpr bool const_size = false;
	static constexpr size_t size     = lak::dynamic_extent;

	static lak::result<lak::span<const byte_t>> from_bytes(
	  lak::from_bytes_data<value_type, E> data)
	{
		lak::binary_reader strm{data.src};
		for (auto &v : data.dst)
		{
			RES_TRY_ASSIGN(auto size =, strm.template read<lak::nbt::TAG_Short>());
			RES_TRY_ASSIGN(
			  v.value =,
			  strm.template read_exact_c_str<char8_t, lak::endian::little>(
			    size.value));
		}
		return lak::ok_t{strm.remaining()};
	}
};

template<lak::endian E>
struct lak::from_bytes_traits<lak::nbt::TAG_List, E>
{
	using value_type                 = lak::nbt::TAG_List;
	static constexpr bool const_size = false;
	static constexpr size_t size     = lak::dynamic_extent;

	static lak::result<lak::span<const byte_t>> from_bytes(
	  lak::from_bytes_data<value_type, E> data)
	{
		lak::binary_reader strm{data.src};
		for (auto &v : data.dst)
		{
			RES_TRY_ASSIGN(auto type =, strm.template read<lak::nbt::tag_type>());
			RES_TRY_ASSIGN(auto size =, strm.template read<lak::nbt::TAG_Int>());
			switch (type)
			{
#define LAK_NBT_READER_VISIT(VAL, NAME, TAG, ...)                             \
	case lak::nbt::tag_type::NAME:                                              \
	{                                                                           \
		RES_TRY_ASSIGN(v.value =, strm.template read<lak::nbt::TAG>(size.value)); \
	}                                                                           \
	break;
				LAK_FOREACH_NBT_TYPE(LAK_NBT_READER_VISIT)
#undef LAK_NBT_READER_VISIT
				default:
					return lak::err_t{};
			}
		}
		return lak::ok_t{strm.remaining()};
	}
};

template<lak::endian E>
struct lak::from_bytes_traits<lak::nbt::TAG_Compound, E>
{
	using value_type                 = lak::nbt::TAG_Compound;
	static constexpr bool const_size = false;
	static constexpr size_t size     = lak::dynamic_extent;

	static lak::result<lak::span<const byte_t>> from_bytes(
	  lak::from_bytes_data<value_type, E> data)
	{
		lak::binary_reader strm{data.src};
		for (auto &v : data.dst)
		{
			for (;;)
			{
				if_let_ok (auto t, strm.template peek<lak::nbt::tag_type>())
				{
					if (t == lak::nbt::tag_type::End)
					{
						strm.template read<lak::nbt::tag_type>().unwrap();
						break;
					}
				}
				else
					return lak::err_t{};

				RES_TRY_ASSIGN(auto tag =, strm.template read<lak::nbt::named_tag>());
				v.value.push_back(lak::move(tag));
			}
		}
		return lak::ok_t{strm.remaining()};
	}
};

static_assert(
  lak::has_from_bytes_traits<lak::nbt::tag_type, lak::endian::little>);
static_assert(
  lak::has_from_bytes_traits<lak::nbt::named_tag, lak::endian::little>);
static_assert(
  lak::has_from_bytes_traits<lak::nbt::TAG_End, lak::endian::little>);
static_assert(
  lak::has_from_bytes_traits<lak::nbt::TAG_Byte, lak::endian::little>);
static_assert(
  lak::has_from_bytes_traits<lak::nbt::TAG_Short, lak::endian::little>);
static_assert(
  lak::has_from_bytes_traits<lak::nbt::TAG_Int, lak::endian::little>);
static_assert(
  lak::has_from_bytes_traits<lak::nbt::TAG_Long, lak::endian::little>);
static_assert(
  lak::has_from_bytes_traits<lak::nbt::TAG_Float, lak::endian::little>);
static_assert(
  lak::has_from_bytes_traits<lak::nbt::TAG_Double, lak::endian::little>);
static_assert(
  lak::has_from_bytes_traits<lak::nbt::TAG_Byte_Array, lak::endian::little>);
static_assert(
  lak::has_from_bytes_traits<lak::nbt::TAG_String, lak::endian::little>);
static_assert(
  lak::has_from_bytes_traits<lak::nbt::TAG_List, lak::endian::little>);
static_assert(
  lak::has_from_bytes_traits<lak::nbt::TAG_Compound, lak::endian::little>);
static_assert(
  lak::has_from_bytes_traits<lak::nbt::TAG_Int_Array, lak::endian::little>);
static_assert(
  lak::has_from_bytes_traits<lak::nbt::TAG_Long_Array, lak::endian::little>);

#endif
