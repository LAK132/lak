#include "lak/bitset.hpp"
#include "lak/stdint.hpp"

#define LAK_TEMPLATE_IS_COLOUR(COLOUR, ...)                                   \
	template<>                                                                  \
	struct lak::is_colour<lak::colour::COLOUR> : lak::true_type                 \
	{                                                                           \
	};
LAK_FOREACH_COLOUR(LAK_TEMPLATE_IS_COLOUR)

#define LAK_TEMPLATE_IS_COLOUR_CHANNEL(CHANNEL, ...)                          \
	template<>                                                                  \
	struct lak::is_colour_channel<lak::colour::CHANNEL> : lak::true_type        \
	{                                                                           \
	};
LAK_FOREACH_VIRTUAL_COLOUR_CHANNEL(LAK_TEMPLATE_IS_COLOUR_CHANNEL)

#define LAK_TEMPLATE_CHANNEL_BIT_COUNT(SYMBOL, NAME, SIZE, ...)               \
	template<>                                                                  \
	struct lak::colour::bit_count<lak::colour::channel_type::NAME##SIZE>        \
	: lak::size_type<SIZE>                                                      \
	{                                                                           \
	};
LAK_FOREACH_SIZED_CHANNEL_TYPE(LAK_TEMPLATE_CHANNEL_BIT_COUNT)

#define LAK_TEMPLATE_CHANNEL_NAME(SYMBOL, NAME, SIZE, ...)                    \
	template<>                                                                  \
	struct lak::colour::channel_name<lak::colour::channel_type::NAME##SIZE>     \
	{                                                                           \
		static constexpr const char *value = #NAME;                               \
	};
LAK_FOREACH_SIZED_CHANNEL_TYPE(LAK_TEMPLATE_CHANNEL_NAME)

template<typename T>
struct lak::colour::common_channel<T> : lak::type_identity<T>
{
};
template<typename T, typename U>
struct lak::colour::common_channel<T, U>
: lak::conditional_t<(T::max_value > U::max_value), T, U>
{
};
template<typename T, typename... U>
struct lak::colour::common_channel<T, U...>
{
	using type =
	  lak::colour::common_channel_t<T, lak::colour::common_channel_t<U...>>;
};

template<typename T>
struct lak::colour::common_colour<T, T>
: lak::type_identity<lak::enable_if_t<lak::is_colour_v<T>, T>>
{
};
#define LAK_DEF_COMMON_COLOUR(C1, C2, COMMON)                                 \
	template<>                                                                  \
	struct lak::colour::common_colour<lak::colour::C1, lak::colour::C2>         \
	: lak::type_identity<lak::colour::COMMON>                                   \
	{                                                                           \
	};                                                                          \
	template<>                                                                  \
	struct lak::colour::common_colour<lak::colour::C2, lak::colour::C1>         \
	: lak::type_identity<lak::colour::COMMON>                                   \
	{                                                                           \
	};
LAK_DEF_COMMON_COLOUR(a8, v8, va88);
LAK_DEF_COMMON_COLOUR(a8, av88, av88);
LAK_DEF_COMMON_COLOUR(a8, va88, va88);
LAK_DEF_COMMON_COLOUR(a8, rgb555, rgba5658);
LAK_DEF_COMMON_COLOUR(a8, rgb565, rgba5658);
LAK_DEF_COMMON_COLOUR(a8, rgb888, rgba8888);
LAK_DEF_COMMON_COLOUR(a8, bgr555, abgr8565);
LAK_DEF_COMMON_COLOUR(a8, bgr565, abgr8565);
LAK_DEF_COMMON_COLOUR(a8, bgr888, abgr8888);
LAK_DEF_COMMON_COLOUR(a8, rgba5551, rgba5658);
LAK_DEF_COMMON_COLOUR(a8, rgba5658, rgba5658);
LAK_DEF_COMMON_COLOUR(a8, rgba8888, rgba8888);
LAK_DEF_COMMON_COLOUR(a8, abgr1555, abgr8565);
LAK_DEF_COMMON_COLOUR(a8, abgr8565, abgr8565);
LAK_DEF_COMMON_COLOUR(a8, abgr8888, abgr8888);

LAK_DEF_COMMON_COLOUR(v8, av88, av88);
LAK_DEF_COMMON_COLOUR(v8, va88, va88);
LAK_DEF_COMMON_COLOUR(v8, rgb555, rgb888);
LAK_DEF_COMMON_COLOUR(v8, rgb565, rgb888);
LAK_DEF_COMMON_COLOUR(v8, rgb888, rgb888);
LAK_DEF_COMMON_COLOUR(v8, bgr555, bgr888);
LAK_DEF_COMMON_COLOUR(v8, bgr565, bgr888);
LAK_DEF_COMMON_COLOUR(v8, bgr888, bgr888);
LAK_DEF_COMMON_COLOUR(v8, rgba5551, rgba8888);
LAK_DEF_COMMON_COLOUR(v8, rgba5658, rgba8888);
LAK_DEF_COMMON_COLOUR(v8, rgba8888, rgba8888);
LAK_DEF_COMMON_COLOUR(v8, abgr1555, abgr8888);
LAK_DEF_COMMON_COLOUR(v8, abgr8565, abgr8888);
LAK_DEF_COMMON_COLOUR(v8, abgr8888, abgr8888);

LAK_DEF_COMMON_COLOUR(av88, va88, va88);
LAK_DEF_COMMON_COLOUR(av88, rgb555, rgba8888);
LAK_DEF_COMMON_COLOUR(av88, rgb565, rgba8888);
LAK_DEF_COMMON_COLOUR(av88, rgb888, rgba8888);
LAK_DEF_COMMON_COLOUR(av88, bgr555, abgr8888);
LAK_DEF_COMMON_COLOUR(av88, bgr565, abgr8888);
LAK_DEF_COMMON_COLOUR(av88, bgr888, abgr8888);
LAK_DEF_COMMON_COLOUR(av88, rgba5551, rgba8888);
LAK_DEF_COMMON_COLOUR(av88, rgba5658, rgba8888);
LAK_DEF_COMMON_COLOUR(av88, rgba8888, rgba8888);
LAK_DEF_COMMON_COLOUR(av88, abgr1555, abgr8888);
LAK_DEF_COMMON_COLOUR(av88, abgr8565, abgr8888);
LAK_DEF_COMMON_COLOUR(av88, abgr8888, abgr8888);

LAK_DEF_COMMON_COLOUR(va88, rgb555, rgba8888);
LAK_DEF_COMMON_COLOUR(va88, rgb565, rgba8888);
LAK_DEF_COMMON_COLOUR(va88, rgb888, rgba8888);
LAK_DEF_COMMON_COLOUR(va88, bgr555, abgr8888);
LAK_DEF_COMMON_COLOUR(va88, bgr565, abgr8888);
LAK_DEF_COMMON_COLOUR(va88, bgr888, abgr8888);
LAK_DEF_COMMON_COLOUR(va88, rgba5551, rgba8888);
LAK_DEF_COMMON_COLOUR(va88, rgba5658, rgba8888);
LAK_DEF_COMMON_COLOUR(va88, rgba8888, rgba8888);
LAK_DEF_COMMON_COLOUR(va88, abgr1555, abgr8888);
LAK_DEF_COMMON_COLOUR(va88, abgr8565, abgr8888);
LAK_DEF_COMMON_COLOUR(va88, abgr8888, abgr8888);

LAK_DEF_COMMON_COLOUR(rgb555, rgb565, rgb565);
LAK_DEF_COMMON_COLOUR(rgb555, rgb888, rgb888);
LAK_DEF_COMMON_COLOUR(rgb555, bgr555, rgb555);
LAK_DEF_COMMON_COLOUR(rgb555, bgr565, bgr565);
LAK_DEF_COMMON_COLOUR(rgb555, bgr888, bgr888);
LAK_DEF_COMMON_COLOUR(rgb555, rgba5551, rgba5551);
LAK_DEF_COMMON_COLOUR(rgb555, rgba5658, rgba5658);
LAK_DEF_COMMON_COLOUR(rgb555, rgba8888, rgba8888);
LAK_DEF_COMMON_COLOUR(rgb555, abgr1555, abgr1555);
LAK_DEF_COMMON_COLOUR(rgb555, abgr8565, abgr8565);
LAK_DEF_COMMON_COLOUR(rgb555, abgr8888, abgr8888);

LAK_DEF_COMMON_COLOUR(rgb565, rgb888, rgb888);
LAK_DEF_COMMON_COLOUR(rgb565, bgr555, rgb565);
LAK_DEF_COMMON_COLOUR(rgb565, bgr565, rgb565);
LAK_DEF_COMMON_COLOUR(rgb565, bgr888, bgr888);
LAK_DEF_COMMON_COLOUR(rgb565, rgba5551, rgba5658);
LAK_DEF_COMMON_COLOUR(rgb565, rgba5658, rgba5658);
LAK_DEF_COMMON_COLOUR(rgb565, rgba8888, rgba8888);
LAK_DEF_COMMON_COLOUR(rgb565, abgr1555, abgr8565);
LAK_DEF_COMMON_COLOUR(rgb565, abgr8565, abgr8565);
LAK_DEF_COMMON_COLOUR(rgb565, abgr8888, abgr8888);

LAK_DEF_COMMON_COLOUR(rgb888, bgr555, rgb888);
LAK_DEF_COMMON_COLOUR(rgb888, bgr565, rgb888);
LAK_DEF_COMMON_COLOUR(rgb888, bgr888, rgb888);
LAK_DEF_COMMON_COLOUR(rgb888, rgba5551, rgba8888);
LAK_DEF_COMMON_COLOUR(rgb888, rgba5658, rgba8888);
LAK_DEF_COMMON_COLOUR(rgb888, rgba8888, rgba8888);
LAK_DEF_COMMON_COLOUR(rgb888, abgr1555, rgba8888);
LAK_DEF_COMMON_COLOUR(rgb888, abgr8565, abgr8888);
LAK_DEF_COMMON_COLOUR(rgb888, abgr8888, abgr8888);

LAK_DEF_COMMON_COLOUR(bgr555, bgr565, bgr565);
LAK_DEF_COMMON_COLOUR(bgr555, bgr888, bgr888);
LAK_DEF_COMMON_COLOUR(bgr555, rgba5551, rgba5551);
LAK_DEF_COMMON_COLOUR(bgr555, rgba5658, rgba5658);
LAK_DEF_COMMON_COLOUR(bgr555, rgba8888, rgba8888);
LAK_DEF_COMMON_COLOUR(bgr555, abgr1555, abgr1555);
LAK_DEF_COMMON_COLOUR(bgr555, abgr8565, abgr8565);
LAK_DEF_COMMON_COLOUR(bgr555, abgr8888, abgr8888);

LAK_DEF_COMMON_COLOUR(bgr565, bgr888, bgr888);
LAK_DEF_COMMON_COLOUR(bgr565, rgba5551, rgba5658);
LAK_DEF_COMMON_COLOUR(bgr565, rgba5658, rgba5658);
LAK_DEF_COMMON_COLOUR(bgr565, rgba8888, rgba8888);
LAK_DEF_COMMON_COLOUR(bgr565, abgr1555, abgr8565);
LAK_DEF_COMMON_COLOUR(bgr565, abgr8565, abgr8565);
LAK_DEF_COMMON_COLOUR(bgr565, abgr8888, abgr8888);

LAK_DEF_COMMON_COLOUR(bgr888, rgba5551, rgba8888);
LAK_DEF_COMMON_COLOUR(bgr888, rgba5658, rgba8888);
LAK_DEF_COMMON_COLOUR(bgr888, rgba8888, rgba8888);
LAK_DEF_COMMON_COLOUR(bgr888, abgr1555, abgr8888);
LAK_DEF_COMMON_COLOUR(bgr888, abgr8565, abgr8888);
LAK_DEF_COMMON_COLOUR(bgr888, abgr8888, abgr8888);

LAK_DEF_COMMON_COLOUR(rgba5551, rgba5658, rgba5658);
LAK_DEF_COMMON_COLOUR(rgba5551, rgba8888, rgba8888);
LAK_DEF_COMMON_COLOUR(rgba5551, abgr1555, rgba5551);
LAK_DEF_COMMON_COLOUR(rgba5551, abgr8565, abgr8565);
LAK_DEF_COMMON_COLOUR(rgba5551, abgr8888, abgr8888);

LAK_DEF_COMMON_COLOUR(rgba5658, rgba8888, rgba8888);
LAK_DEF_COMMON_COLOUR(rgba5658, abgr1555, rgba5658);
LAK_DEF_COMMON_COLOUR(rgba5658, abgr8565, abgr8565);
LAK_DEF_COMMON_COLOUR(rgba5658, abgr8888, abgr8888);

LAK_DEF_COMMON_COLOUR(rgba8888, abgr1555, rgba8888);
LAK_DEF_COMMON_COLOUR(rgba8888, abgr8565, rgba8888);
LAK_DEF_COMMON_COLOUR(rgba8888, abgr8888, rgba8888);

LAK_DEF_COMMON_COLOUR(abgr1555, abgr8565, abgr8565);
LAK_DEF_COMMON_COLOUR(abgr1555, abgr8888, abgr8888);

LAK_DEF_COMMON_COLOUR(abgr8565, abgr8888, abgr8888);
#undef LAK_DEF_COMMON_COLOUR

namespace lak
{
	namespace colour
	{
// We can replace this with nice stuff when we finally get a C++20 compiler
#define LAK_COLOUR_DEFAULTS(TYPE, ...)                                        \
	TYPE()                        = default;                                    \
	TYPE(const TYPE &)            = default;                                    \
	TYPE &operator=(const TYPE &) = default;                                    \
	force_inline bool operator==(const TYPE &rhs) const { return __VA_ARGS__; } \
	force_inline bool operator!=(const TYPE &rhs) const                         \
	{                                                                           \
		return !operator==(rhs);                                                  \
	}
#define LAK_RGB_COLOUR_DEFAULTS(TYPE)                                         \
	LAK_COLOUR_DEFAULTS(TYPE, r == rhs.r && g == rhs.g && b == rhs.b)
#define LAK_RGBA_COLOUR_DEFAULTS(TYPE)                                        \
	LAK_COLOUR_DEFAULTS(TYPE,                                                   \
	                    r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a)

		/* --- channel helper types --- */

		struct channel0
		{
			// _v determines whether this channel should bias towards max_value or 0,
			// is is not the actual value of this channel (this is no-channel
			// channel).
			uint8_t _v : 1;

			static constexpr uint8_t size      = 0;
			static constexpr uint8_t max_value = 0x00U;

			LAK_COLOUR_DEFAULTS(channel0, ((void)rhs, true))

			force_inline channel0(uint8_t V) : _v(V & max_value) {}
			template<typename CHANNEL,
			         lak::enable_if_i<lak::is_colour_channel_v<CHANNEL>> = 0>
			force_inline channel0(CHANNEL c)
			{
			}

			force_inline uint8_t
			to1bit() const; // { return _v * channel1::max_value; }
			force_inline uint8_t
			to5bit() const; // { return _v * channel5::max_value; }
			force_inline uint8_t
			to6bit() const; // { return _v * channel6::max_value; }
			force_inline uint8_t
			to8bit() const; // { return _v * channel8::max_value; }
		};

		template<>
		struct channel_<0> : lak::type_identity<channel0>
		{
		};

		struct channel1
		{
			uint8_t v : 1;

			static constexpr uint8_t size      = 1;
			static constexpr uint8_t max_value = 0x01U;

			LAK_COLOUR_DEFAULTS(channel1, v == rhs.v)

			force_inline channel1(uint8_t V) : v(V & max_value) {}
			template<typename CHANNEL,
			         lak::enable_if_i<lak::is_colour_channel_v<CHANNEL>> = 0>
			force_inline channel1(CHANNEL c) : v(c.to1bit())
			{
			}

			force_inline uint8_t to1bit() const; // { return v; }
			force_inline uint8_t
			to5bit() const; // { return v * channel5::max_value; }
			force_inline uint8_t
			to6bit() const; // { return v * channel6::max_value; }
			force_inline uint8_t
			to8bit() const; // { return v * channel8::max_value; }
		};
		template<>
		struct channel_<1> : lak::type_identity<channel1>
		{
		};

		struct channel5
		{
			uint8_t v : 5;

			static constexpr uint8_t size      = 5;
			static constexpr uint8_t max_value = 0x1FU;

			LAK_COLOUR_DEFAULTS(channel5, v == rhs.v)

			force_inline channel5(uint8_t V) : v(V & max_value) {}
			template<typename CHANNEL,
			         lak::enable_if_i<lak::is_colour_channel_v<CHANNEL>> = 0>
			force_inline channel5(CHANNEL c) : v(c.to5bit())
			{
			}

			force_inline uint8_t to1bit() const; // { return v > 0; }
			force_inline uint8_t to5bit() const; // { return v; }
			force_inline uint8_t to6bit() const; // { return v << 1; }
			force_inline uint8_t to8bit() const; // { return v << 3; }
		};
		template<>
		struct channel_<5> : lak::type_identity<channel5>
		{
		};

		struct channel6
		{
			uint8_t v : 6;

			static constexpr uint8_t size      = 6;
			static constexpr uint8_t max_value = 0x3FU;

			LAK_COLOUR_DEFAULTS(channel6, v == rhs.v)

			force_inline channel6(uint8_t V) : v(V & max_value) {}
			template<typename CHANNEL,
			         lak::enable_if_i<lak::is_colour_channel_v<CHANNEL>> = 0>
			force_inline channel6(CHANNEL c) : v(c.to6bit())
			{
			}

			force_inline uint8_t to1bit() const; // { return v > 0; }
			force_inline uint8_t to5bit() const; // { return v >> 1; }
			force_inline uint8_t to6bit() const; // { return v; }
			force_inline uint8_t to8bit() const; // { return v << 2; }
		};
		template<>
		struct channel_<6> : lak::type_identity<channel6>
		{
		};

		struct channel8
		{
			uint8_t v : 8;

			static constexpr uint8_t size      = 8;
			static constexpr uint8_t max_value = 0xFFU;

			LAK_COLOUR_DEFAULTS(channel8, v == rhs.v)

			force_inline channel8(uint8_t V) : v(V & max_value) {}
			template<typename CHANNEL,
			         lak::enable_if_i<lak::is_colour_channel_v<CHANNEL>> = 0>
			force_inline channel8(CHANNEL c) : v(c.to8bit())
			{
			}

			force_inline uint8_t to1bit() const; // { return v > 0; }
			force_inline uint8_t to5bit() const; // { return v >> 3; }
			force_inline uint8_t to6bit() const; // { return v >> 2; }
			force_inline uint8_t to8bit() const; // { return v; }
		};
		template<>
		struct channel_<8> : lak::type_identity<channel8>
		{
		};

		force_inline uint8_t channel0::to1bit() const
		{
			return _v * channel1::max_value;
		}
		force_inline uint8_t channel0::to5bit() const
		{
			return _v * channel5::max_value;
		}
		force_inline uint8_t channel0::to6bit() const
		{
			return _v * channel6::max_value;
		}
		force_inline uint8_t channel0::to8bit() const
		{
			return _v * channel8::max_value;
		}

		force_inline uint8_t channel1::to1bit() const { return v; }
		force_inline uint8_t channel1::to5bit() const
		{
			return v * channel5::max_value;
		}
		force_inline uint8_t channel1::to6bit() const
		{
			return v * channel6::max_value;
		}
		force_inline uint8_t channel1::to8bit() const
		{
			return v * channel8::max_value;
		}

		force_inline uint8_t channel5::to1bit() const { return v > 0; }
		force_inline uint8_t channel5::to5bit() const { return v; }
		force_inline uint8_t channel5::to6bit() const { return v << 1; }
		force_inline uint8_t channel5::to8bit() const { return v << 3; }

		force_inline uint8_t channel6::to1bit() const { return v > 0; }
		force_inline uint8_t channel6::to5bit() const { return v >> 1; }
		force_inline uint8_t channel6::to6bit() const { return v; }
		force_inline uint8_t channel6::to8bit() const { return v << 2; }

		force_inline uint8_t channel8::to1bit() const { return v > 0; }
		force_inline uint8_t channel8::to5bit() const { return v >> 3; }
		force_inline uint8_t channel8::to6bit() const { return v >> 2; }
		force_inline uint8_t channel8::to8bit() const { return v; }

		/* --- indexed --- */

		struct i8
		{
			uint8_t i;

			LAK_COLOUR_DEFAULTS(i8, i == rhs.i)

			i8(uint8_t I) : i(I) {}

			force_inline channel8 I() const { return {i}; }

			force_inline void R(channel8 c) { i = c.v; }

			// This one needs access to the palette to do conversion.
		};
		static_assert(sizeof(i8) == 8 / 8);
		template<>
		struct is_indexed<lak::colour::i8> : lak::true_type
		{
		};

		/* --- pure alpha --- */

		packed_struct a8
		{
			uint8_t a;

			LAK_COLOUR_DEFAULTS(a8, a == rhs.a)

			a8(uint8_t A) : a(A) {}

			force_inline channel0 R() const { return {1}; }
			force_inline channel0 G() const { return {1}; }
			force_inline channel0 B() const { return {1}; }
			force_inline channel8 A() const { return {a}; }
			force_inline channel0 V() const { return {1}; }

			force_inline void R(channel0) {}
			force_inline void G(channel0) {}
			force_inline void B(channel0) {}
			force_inline void A(channel8 c) { a = c.v; }
			force_inline void V(channel0) {}

			// template<typename C, lak::enable_if_i<lak::is_colour_v<C>> = 0>
			template<typename C>
			force_inline explicit a8(const C &c)
			{
				A(c.A());
			}
		};
		static_assert(sizeof(a8) == 8 / 8);
		template<>
		struct has_alpha<a8> : lak::true_type
		{
		};

		/* --- monochrome --- */

		packed_struct v8
		{
			uint8_t v;

			LAK_COLOUR_DEFAULTS(v8, v == rhs.v)

			v8(uint8_t V) : v(V) {}

			// :TODO: having these as channel8 will incorrectly mark this as not
			// monochrome.
			force_inline channel8 R() const { return {v}; }
			force_inline channel8 G() const { return {v}; }
			force_inline channel8 B() const { return {v}; }
			force_inline channel0 A() const { return {1}; }
			force_inline channel8 V() const { return {v}; }

			force_inline void R(channel0) {}
			force_inline void G(channel0) {}
			force_inline void B(channel0) {}
			force_inline void A(channel0) {}
			force_inline void V(channel8 c) { v = c.v; }

			// template<typename C, lak::enable_if_i<lak::is_colour_v<C>> = 0>
			template<typename C>
			force_inline explicit v8(const C &c)
			{
				V(c.V());
			}
		};
		static_assert(sizeof(v8) == 8 / 8);
		template<>
		struct is_monochrome<v8> : lak::true_type
		{
		};

		/* --- alpha + monochrome --- */

		packed_struct av88
		{
			uint8_t a;
			uint8_t v;

			LAK_COLOUR_DEFAULTS(av88, a == rhs.a && v == rhs.v)

			av88(uint16_t AV) : a(uint8_t(AV >> 8)), v(uint8_t(AV)) {}

			av88(uint8_t A, uint8_t V) : a(A), v(V) {}

			// :TODO: having these as channel8 will incorrectly mark this as not
			// monochrome.
			force_inline channel8 R() const { return {v}; }
			force_inline channel8 G() const { return {v}; }
			force_inline channel8 B() const { return {v}; }
			force_inline channel8 A() const { return {a}; }
			force_inline channel8 V() const { return {v}; }

			force_inline void R(channel0) {}
			force_inline void G(channel0) {}
			force_inline void B(channel0) {}
			force_inline void A(channel8 c) { a = c.v; }
			force_inline void V(channel8 c) { v = c.v; }

			// template<typename C, lak::enable_if_i<lak::is_colour_v<C>> = 0>
			template<typename C>
			force_inline explicit av88(const C &c)
			{
				V(c.V());
				A(c.A());
			}
		};
		static_assert(sizeof(av88) == 16 / 8);
		template<>
		struct has_alpha<av88> : lak::true_type
		{
		};
		template<>
		struct is_monochrome<av88> : lak::true_type
		{
		};

		packed_struct va88
		{
			uint8_t v;
			uint8_t a;

			LAK_COLOUR_DEFAULTS(va88, a == rhs.a && v == rhs.v)

			va88(uint16_t VA) : v(uint8_t(VA >> 8)), a(uint8_t(VA)) {}

			va88(uint8_t V, uint8_t A) : v(V), a(A) {}

			// :TODO: having these as channel8 will incorrectly mark this as not
			// monochrome.
			force_inline channel8 R() const { return {v}; }
			force_inline channel8 G() const { return {v}; }
			force_inline channel8 B() const { return {v}; }
			force_inline channel8 A() const { return {a}; }
			force_inline channel8 V() const { return {v}; }

			force_inline void R(channel0) {}
			force_inline void G(channel0) {}
			force_inline void B(channel0) {}
			force_inline void A(channel8 c) { a = c.v; }
			force_inline void V(channel8 c) { v = c.v; }

			// template<typename C, lak::enable_if_i<lak::is_colour_v<C>> = 0>
			template<typename C>
			force_inline explicit va88(const C &c)
			{
				V(c.V());
				A(c.A());
			}
		};
		static_assert(sizeof(va88) == 16 / 8);
		template<>
		struct has_alpha<va88> : lak::true_type
		{
		};
		template<>
		struct is_monochrome<va88> : lak::true_type
		{
		};

		/* --- RGB --- */

		packed_struct rgb555
		{
			lak::native_bitset<uint8_t, 5, 5, 5> rgb;

			LAK_COLOUR_DEFAULTS(rgb555, rgb == rhs.rgb)

			rgb555(uint8_t R, uint8_t G, uint8_t B)
			{
				rgb.set<0>(R);
				rgb.set<1>(G);
				rgb.set<2>(B);
			}

			force_inline channel5 R() const { return {rgb.get<0>()}; }
			force_inline channel5 G() const { return {rgb.get<1>()}; }
			force_inline channel5 B() const { return {rgb.get<2>()}; }
			force_inline channel0 A() const { return {1}; }
			force_inline channel5 V() const
			{
				return {static_cast<uint8_t>((R().v + G().v + B().v) / 3)};
			}

			force_inline void R(channel5 c) { rgb.set<0>(c.v); }
			force_inline void G(channel5 c) { rgb.set<1>(c.v); }
			force_inline void B(channel5 c) { rgb.set<2>(c.v); }
			force_inline void A(channel0) {}
			force_inline void V(channel5 c)
			{
				rgb.set<0>(c.v);
				rgb.set<1>(c.v);
				rgb.set<2>(c.v);
			}

			// template<typename C, lak::enable_if_i<lak::is_colour_v<C>> = 0>
			template<typename C>
			force_inline explicit rgb555(const C &c)
			{
				R(c.R());
				G(c.G());
				B(c.B());
			}
		};
		static_assert(sizeof(rgb555) == 16 / 8);
		template<>
		struct has_red<rgb555> : lak::true_type
		{
		};
		template<>
		struct has_green<rgb555> : lak::true_type
		{
		};
		template<>
		struct has_blue<rgb555> : lak::true_type
		{
		};

		packed_struct rgb565
		{
			lak::native_bitset<uint8_t, 5, 6, 5> rgb;

			LAK_COLOUR_DEFAULTS(rgb565, rgb == rhs.rgb)

			rgb565(uint8_t R, uint8_t G, uint8_t B)
			{
				rgb.set<0>(R);
				rgb.set<1>(G);
				rgb.set<2>(B);
			}

			force_inline channel5 R() const { return {rgb.get<0>()}; }
			force_inline channel6 G() const { return {rgb.get<1>()}; }
			force_inline channel5 B() const { return {rgb.get<2>()}; }
			force_inline channel0 A() const { return {1}; }
			force_inline channel8 V() const
			{
				return {static_cast<uint8_t>(
				  (R().to8bit() + G().to8bit() + B().to8bit()) / 3)};
			}

			force_inline void R(channel5 c) { rgb.set<0>(c.v); }
			force_inline void G(channel6 c) { rgb.set<1>(c.v); }
			force_inline void B(channel5 c) { rgb.set<2>(c.v); }
			force_inline void A(channel0) {}
			force_inline void V(channel8 c)
			{
				const auto c5 = c.to5bit();
				rgb.set<0>(c5);
				rgb.set<1>(c.to6bit());
				rgb.set<2>(c5);
			}

			// template<typename C, lak::enable_if_i<lak::is_colour_v<C>> = 0>
			template<typename C>
			force_inline explicit rgb565(const C &c)
			{
				R(c.R());
				G(c.G());
				B(c.B());
			}
		};
		static_assert(sizeof(rgb565) == 16 / 8);
		template<>
		struct has_red<rgb565> : lak::true_type
		{
		};
		template<>
		struct has_green<rgb565> : lak::true_type
		{
		};
		template<>
		struct has_blue<rgb565> : lak::true_type
		{
		};

		packed_struct rgb888
		{
			uint8_t r;
			uint8_t g;
			uint8_t b;

			LAK_RGB_COLOUR_DEFAULTS(rgb888)

			rgb888(uint32_t RGB)
			: r((RGB >> 16) & 0xFFU), g((RGB >> 8) & 0xFFU), b(RGB & 0xFFU)
			{
			}

			rgb888(uint8_t R, uint8_t G, uint8_t B) : r(R), g(G), b(B) {}

			force_inline channel8 R() const { return {r}; }
			force_inline channel8 G() const { return {g}; }
			force_inline channel8 B() const { return {b}; }
			force_inline channel0 A() const { return {1}; }
			force_inline channel8 V() const
			{
				return {static_cast<uint8_t>((r + g + b) / 3)};
			}

			force_inline void R(channel8 c) { r = c.v; }
			force_inline void G(channel8 c) { g = c.v; }
			force_inline void B(channel8 c) { b = c.v; }
			force_inline void A(channel0) {}
			force_inline void V(channel0) {}

			// template<typename C, lak::enable_if_i<lak::is_colour_v<C>> = 0>
			template<typename C>
			force_inline explicit rgb888(const C &c)
			{
				R(c.R());
				G(c.G());
				B(c.B());
			}
		};
		static_assert(sizeof(rgb888) == 24 / 8);
		template<>
		struct has_red<rgb888> : lak::true_type
		{
		};
		template<>
		struct has_green<rgb888> : lak::true_type
		{
		};
		template<>
		struct has_blue<rgb888> : lak::true_type
		{
		};

		/* --- BGR --- */

		packed_struct bgr555
		{
			lak::native_bitset<uint8_t, 5, 5, 5> bgr;

			LAK_COLOUR_DEFAULTS(bgr555, bgr == rhs.bgr)

			bgr555(uint8_t B, uint8_t G, uint8_t R)
			{
				bgr.set<0>(B);
				bgr.set<1>(G);
				bgr.set<2>(R);
			}

			force_inline channel5 R() const { return {bgr.get<2>()}; }
			force_inline channel5 G() const { return {bgr.get<1>()}; }
			force_inline channel5 B() const { return {bgr.get<0>()}; }
			force_inline channel0 A() const { return {1}; }
			force_inline channel5 V() const
			{
				return {static_cast<uint8_t>((R().v + G().v + B().v) / 3)};
			}

			force_inline void R(channel5 c) { bgr.set<2>(c.v); }
			force_inline void G(channel5 c) { bgr.set<1>(c.v); }
			force_inline void B(channel5 c) { bgr.set<0>(c.v); }
			force_inline void A(channel0) {}
			force_inline void V(channel5 c)
			{
				bgr.set<0>(c.v);
				bgr.set<1>(c.v);
				bgr.set<2>(c.v);
			}

			// template<typename C, lak::enable_if_i<lak::is_colour_v<C>> = 0>
			template<typename C>
			force_inline explicit bgr555(const C &c)
			{
				B(c.B());
				G(c.G());
				R(c.R());
			}
		};
		static_assert(sizeof(bgr555) == 16 / 8);
		template<>
		struct has_red<bgr555> : lak::true_type
		{
		};
		template<>
		struct has_green<bgr555> : lak::true_type
		{
		};
		template<>
		struct has_blue<bgr555> : lak::true_type
		{
		};

		packed_struct bgr565
		{
			lak::native_bitset<uint8_t, 5, 6, 5> bgr;

			LAK_COLOUR_DEFAULTS(bgr565, bgr == rhs.bgr)

			bgr565(uint8_t B, uint8_t G, uint8_t R)
			{
				bgr.set<2>(B);
				bgr.set<1>(G);
				bgr.set<0>(R);
			}

			force_inline channel5 R() const { return {bgr.get<2>()}; }
			force_inline channel6 G() const { return {bgr.get<1>()}; }
			force_inline channel5 B() const { return {bgr.get<0>()}; }
			force_inline channel0 A() const { return {1}; }
			force_inline channel8 V() const
			{
				return {static_cast<uint8_t>(
				  (R().to8bit() + G().to8bit() + B().to8bit()) / 3)};
			}

			force_inline void R(channel5 c) { bgr.set<2>(c.v); }
			force_inline void G(channel6 c) { bgr.set<1>(c.v); }
			force_inline void B(channel5 c) { bgr.set<0>(c.v); }
			force_inline void A(channel0) {}
			force_inline void V(channel8 c)
			{
				const auto c5 = c.to5bit();
				bgr.set<2>(c5);
				bgr.set<1>(c.to6bit());
				bgr.set<0>(c5);
			}

			// template<typename C, lak::enable_if_i<lak::is_colour_v<C>> = 0>
			template<typename C>
			force_inline explicit bgr565(const C &c)
			{
				B(c.B());
				G(c.G());
				R(c.R());
			}
		};
		static_assert(sizeof(bgr565) == 16 / 8);
		template<>
		struct has_red<bgr565> : lak::true_type
		{
		};
		template<>
		struct has_green<bgr565> : lak::true_type
		{
		};
		template<>
		struct has_blue<bgr565> : lak::true_type
		{
		};

		packed_struct bgr888
		{
			uint8_t b;
			uint8_t g;
			uint8_t r;

			LAK_RGB_COLOUR_DEFAULTS(bgr888)

			bgr888(uint32_t BGR)
			: b((BGR >> 16) & 0xFFU), g((BGR >> 8) & 0xFFU), r(BGR & 0xFFU)
			{
			}

			bgr888(uint8_t B, uint8_t G, uint8_t R) : b(B), g(G), r(R) {}

			force_inline channel8 R() const { return {r}; }
			force_inline channel8 G() const { return {g}; }
			force_inline channel8 B() const { return {b}; }
			force_inline channel0 A() const { return {1}; }
			force_inline channel8 V() const
			{
				return {static_cast<uint8_t>((r + g + b) / 3)};
			}

			force_inline void R(channel8 c) { r = c.v; }
			force_inline void G(channel8 c) { g = c.v; }
			force_inline void B(channel8 c) { b = c.v; }
			force_inline void A(channel0) {}
			force_inline void V(channel0) {}

			// template<typename C, lak::enable_if_i<lak::is_colour_v<C>> = 0>
			template<typename C>
			force_inline explicit bgr888(const C &c)
			{
				R(c.R());
				G(c.G());
				B(c.B());
			}
		};
		static_assert(sizeof(bgr888) == 24 / 8);
		template<>
		struct has_red<bgr888> : lak::true_type
		{
		};
		template<>
		struct has_green<bgr888> : lak::true_type
		{
		};
		template<>
		struct has_blue<bgr888> : lak::true_type
		{
		};

		packed_struct bgrx8888
		{
			uint8_t b;
			uint8_t g;
			uint8_t r;
			uint8_t x;

			LAK_RGB_COLOUR_DEFAULTS(bgrx8888)

			bgrx8888(uint32_t BGR)
			: b((BGR >> 16) & 0xFFU), g((BGR >> 8) & 0xFFU), r(BGR & 0xFFU)
			{
			}

			bgrx8888(uint8_t B, uint8_t G, uint8_t R) : b(B), g(G), r(R) {}

			force_inline channel8 R() const { return {r}; }
			force_inline channel8 G() const { return {g}; }
			force_inline channel8 B() const { return {b}; }
			force_inline channel0 A() const { return {1}; }
			force_inline channel8 V() const
			{
				return {static_cast<uint8_t>((r + g + b) / 3)};
			}

			force_inline void R(channel8 c) { r = c.v; }
			force_inline void G(channel8 c) { g = c.v; }
			force_inline void B(channel8 c) { b = c.v; }
			force_inline void A(channel0) {}
			force_inline void V(channel0) {}

			// template<typename C, lak::enable_if_i<lak::is_colour_v<C>> = 0>
			template<typename C>
			force_inline explicit bgrx8888(const C &c)
			{
				R(c.R());
				G(c.G());
				B(c.B());
			}
		};
		static_assert(sizeof(bgrx8888) == 32 / 8);
		template<>
		struct has_red<bgrx8888> : lak::true_type
		{
		};
		template<>
		struct has_green<bgrx8888> : lak::true_type
		{
		};
		template<>
		struct has_blue<bgrx8888> : lak::true_type
		{
		};

		/* --- RGBA --- */

		packed_struct rgba5551
		{
			lak::native_bitset<uint8_t, 5, 5, 5, 1> rgba;

			LAK_COLOUR_DEFAULTS(rgba5551, rgba == rhs.rgba)

			rgba5551(uint8_t R, uint8_t G, uint8_t B, uint8_t A)
			{
				rgba.set<0>(R);
				rgba.set<1>(G);
				rgba.set<2>(B);
				rgba.set<3>(A);
			}

			force_inline channel5 R() const { return {rgba.get<0>()}; }
			force_inline channel5 G() const { return {rgba.get<1>()}; }
			force_inline channel5 B() const { return {rgba.get<2>()}; }
			force_inline channel1 A() const { return {rgba.get<3>()}; }
			force_inline channel5 V() const
			{
				return {static_cast<uint8_t>((R().v + G().v + B().v) / 3)};
			}

			force_inline void R(channel5 c) { rgba.set<0>(c.v); }
			force_inline void G(channel5 c) { rgba.set<1>(c.v); }
			force_inline void B(channel5 c) { rgba.set<2>(c.v); }
			force_inline void A(channel1 c) { rgba.set<3>(c.v); }
			force_inline void V(channel5 c)
			{
				rgba.set<0>(c.v);
				rgba.set<1>(c.v);
				rgba.set<2>(c.v);
			}

			// template<typename C, lak::enable_if_i<lak::is_colour_v<C>> = 0>
			template<typename C>
			force_inline explicit rgba5551(const C &c)
			{
				R(c.R());
				G(c.G());
				B(c.B());
				A(c.A());
			}
		};
		static_assert(sizeof(rgba5551) == 16 / 8);
		template<>
		struct has_red<rgba5551> : lak::true_type
		{
		};
		template<>
		struct has_green<rgba5551> : lak::true_type
		{
		};
		template<>
		struct has_blue<rgba5551> : lak::true_type
		{
		};
		template<>
		struct has_alpha<rgba5551> : lak::true_type
		{
		};

		packed_struct rgba5658
		{
			lak::native_bitset<uint8_t, 5, 6, 5> rgb;
			uint8_t a;

			LAK_COLOUR_DEFAULTS(rgba5658, rgb == rhs.rgb && a == rhs.a)

			rgba5658(uint8_t R, uint8_t G, uint8_t B, uint8_t A)
			{
				rgb.set<0>(R);
				rgb.set<1>(G);
				rgb.set<2>(B);
				a = A;
			}

			force_inline channel5 R() const { return {rgb.get<0>()}; }
			force_inline channel6 G() const { return {rgb.get<1>()}; }
			force_inline channel5 B() const { return {rgb.get<2>()}; }
			force_inline channel8 A() const { return {a}; }
			force_inline channel8 V() const
			{
				return {static_cast<uint8_t>(
				  (R().to8bit() + G().to8bit() + B().to8bit()) / 3)};
			}

			force_inline void R(channel5 c) { rgb.set<0>(c.v); }
			force_inline void G(channel5 c) { rgb.set<1>(c.v); }
			force_inline void B(channel5 c) { rgb.set<2>(c.v); }
			force_inline void A(channel1 c) { a = c.v; }
			force_inline void V(channel5 c)
			{
				rgb.set<0>(c.v);
				rgb.set<1>(c.v);
				rgb.set<2>(c.v);
			}

			// template<typename C, lak::enable_if_i<lak::is_colour_v<C>> = 0>
			template<typename C>
			force_inline explicit rgba5658(const C &c)
			{
				R(c.R());
				G(c.G());
				B(c.B());
				A(c.A());
			}
		};
		static_assert(sizeof(rgba5658) == 24 / 8);
		template<>
		struct has_red<rgba5658> : lak::true_type
		{
		};
		template<>
		struct has_green<rgba5658> : lak::true_type
		{
		};
		template<>
		struct has_blue<rgba5658> : lak::true_type
		{
		};
		template<>
		struct has_alpha<rgba5658> : lak::true_type
		{
		};

		packed_struct rgba8888
		{
			uint8_t r;
			uint8_t g;
			uint8_t b;
			uint8_t a;

			LAK_RGBA_COLOUR_DEFAULTS(rgba8888)

			rgba8888(uint32_t RGBA)
			: r((RGBA >> 24) & 0xFFU),
			  g((RGBA >> 16) & 0xFFU),
			  b((RGBA >> 8) & 0xFFU),
			  a(RGBA & 0xFFU)
			{
			}

			rgba8888(uint8_t R, uint8_t G, uint8_t B, uint8_t A)
			: r(R), g(G), b(B), a(A)
			{
			}

			force_inline channel8 R() const { return {r}; }
			force_inline channel8 G() const { return {g}; }
			force_inline channel8 B() const { return {b}; }
			force_inline channel8 A() const { return {a}; }
			force_inline channel8 V() const
			{
				return {static_cast<uint8_t>((r + g + b) / 3)};
			}

			force_inline void R(channel8 c) { r = c.v; }
			force_inline void G(channel8 c) { g = c.v; }
			force_inline void B(channel8 c) { b = c.v; }
			force_inline void A(channel8 c) { a = c.v; }
			force_inline void V(channel0) {}

			// template<typename C, lak::enable_if_i<lak::is_colour_v<C>> = 0>
			template<typename C>
			force_inline explicit rgba8888(const C &c)
			{
				R(c.R());
				G(c.G());
				B(c.B());
				A(c.A());
			}
		};
		static_assert(sizeof(rgba8888) == 32 / 8);
		template<>
		struct has_red<rgba8888> : lak::true_type
		{
		};
		template<>
		struct has_green<rgba8888> : lak::true_type
		{
		};
		template<>
		struct has_blue<rgba8888> : lak::true_type
		{
		};
		template<>
		struct has_alpha<rgba8888> : lak::true_type
		{
		};

		/* --- ABGR --- */

		packed_struct abgr1555
		{
			lak::native_bitset<uint8_t, 1, 5, 5, 5> abgr;

			LAK_COLOUR_DEFAULTS(abgr1555, abgr == rhs.abgr)

			abgr1555(uint8_t A, uint8_t B, uint8_t G, uint8_t R)
			{
				abgr.set<0>(A);
				abgr.set<1>(B);
				abgr.set<2>(G);
				abgr.set<3>(R);
			}

			force_inline channel5 R() const { return {abgr.get<3>()}; }
			force_inline channel5 G() const { return {abgr.get<2>()}; }
			force_inline channel5 B() const { return {abgr.get<1>()}; }
			force_inline channel1 A() const { return {abgr.get<0>()}; }
			force_inline channel5 V() const
			{
				return {static_cast<uint8_t>((R().v + G().v + B().v) / 3)};
			}

			force_inline void R(channel5 c) { abgr.set<3>(c.v); }
			force_inline void G(channel5 c) { abgr.set<2>(c.v); }
			force_inline void B(channel5 c) { abgr.set<1>(c.v); }
			force_inline void A(channel1 c) { abgr.set<0>(c.v); }
			force_inline void V(channel5 c)
			{
				abgr.set<3>(c.v);
				abgr.set<2>(c.v);
				abgr.set<1>(c.v);
			}

			// template<typename C, lak::enable_if_i<lak::is_colour_v<C>> = 0>
			template<typename C>
			force_inline explicit abgr1555(const C &c)
			{
				R(c.R());
				G(c.G());
				B(c.B());
				A(c.A());
			}
		};
		static_assert(sizeof(abgr1555) == 16 / 8);
		template<>
		struct has_red<abgr1555> : lak::true_type
		{
		};
		template<>
		struct has_green<abgr1555> : lak::true_type
		{
		};
		template<>
		struct has_blue<abgr1555> : lak::true_type
		{
		};
		template<>
		struct has_alpha<abgr1555> : lak::true_type
		{
		};

		packed_struct abgr8565
		{
			uint8_t a;
			lak::native_bitset<uint8_t, 5, 5, 5> bgr;

			LAK_COLOUR_DEFAULTS(abgr8565, a == rhs.a && bgr == rhs.bgr)

			abgr8565(uint8_t A, uint8_t B, uint8_t G, uint8_t R)
			{
				a = A;
				bgr.set<0>(B);
				bgr.set<1>(G);
				bgr.set<2>(R);
			}

			force_inline channel5 R() const { return {bgr.get<2>()}; }
			force_inline channel6 G() const { return {bgr.get<1>()}; }
			force_inline channel5 B() const { return {bgr.get<0>()}; }
			force_inline channel8 A() const { return {a}; }
			force_inline channel8 V() const
			{
				return {static_cast<uint8_t>((R().v + G().v + B().v) / 3)};
			}

			force_inline void R(channel5 c) { bgr.set<2>(c.v); }
			force_inline void G(channel6 c) { bgr.set<1>(c.v); }
			force_inline void B(channel5 c) { bgr.set<0>(c.v); }
			force_inline void A(channel8 c) { a = c.v; }
			force_inline void V(channel8 c)
			{
				bgr.set<2>(c.v);
				bgr.set<1>(c.v);
				bgr.set<0>(c.v);
			}

			// template<typename C, lak::enable_if_i<lak::is_colour_v<C>> = 0>
			template<typename C>
			force_inline explicit abgr8565(const C &c)
			{
				R(c.R());
				G(c.G());
				B(c.B());
				A(c.A());
			}
		};
		static_assert(sizeof(abgr8565) == 24 / 8);
		template<>
		struct has_red<abgr8565> : lak::true_type
		{
		};
		template<>
		struct has_green<abgr8565> : lak::true_type
		{
		};
		template<>
		struct has_blue<abgr8565> : lak::true_type
		{
		};
		template<>
		struct has_alpha<abgr8565> : lak::true_type
		{
		};

		packed_struct abgr8888
		{
			uint8_t a;
			uint8_t b;
			uint8_t g;
			uint8_t r;

			LAK_RGBA_COLOUR_DEFAULTS(abgr8888)

			abgr8888(uint32_t ABGR)
			: a((ABGR >> 24) & 0xFFU),
			  b((ABGR >> 16) & 0xFFU),
			  g((ABGR >> 8) & 0xFFU),
			  r(ABGR & 0xFFU)
			{
			}

			abgr8888(uint8_t A, uint8_t B, uint8_t G, uint8_t R)
			: a(A), b(B), g(G), r(R)
			{
			}

			force_inline channel8 R() const { return {r}; }
			force_inline channel8 G() const { return {g}; }
			force_inline channel8 B() const { return {b}; }
			force_inline channel8 A() const { return {a}; }
			force_inline channel8 V() const
			{
				return {static_cast<uint8_t>((r + g + b) / 3)};
			}

			force_inline void R(channel8 c) { r = c.v; }
			force_inline void G(channel8 c) { g = c.v; }
			force_inline void B(channel8 c) { b = c.v; }
			force_inline void A(channel8 c) { a = c.v; }
			force_inline void V(channel0) {}

			// template<typename C, lak::enable_if_i<lak::is_colour_v<C>> = 0>
			template<typename C>
			force_inline explicit abgr8888(const C &c)
			{
				R(c.R());
				G(c.G());
				B(c.B());
				A(c.A());
			}
		};
		static_assert(sizeof(abgr8888) == 32 / 8);
		template<>
		struct has_red<abgr8888> : lak::true_type
		{
		};
		template<>
		struct has_green<abgr8888> : lak::true_type
		{
		};
		template<>
		struct has_blue<abgr8888> : lak::true_type
		{
		};
		template<>
		struct has_alpha<abgr8888> : lak::true_type
		{
		};

#undef LAK_COLOUR_DEFAULTS
#undef LAK_RGB_COLOUR_DEFAULTS
#undef LAK_RGBA_COLOUR_DEFAULTS
	}
}
