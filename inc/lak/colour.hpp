#ifndef LAK_COLOUR_HPP
#define LAK_COLOUR_HPP

#include "lak/compiler.hpp"
#include "lak/type_traits.hpp"

namespace lak
{
	DEFAULTED_TEMPLATE_VALUE_TYPE(is_colour, typename, bool, false);

	DEFAULTED_TEMPLATE_VALUE_TYPE(is_colour_channel, typename, bool, false);

	namespace colour
	{
		// true if the colour type has a red (.r) channel.
		DEFAULTED_TEMPLATE_VALUE_TYPE(has_red, typename, bool, false);

		// true if the colour type has a green (.g) channel.
		DEFAULTED_TEMPLATE_VALUE_TYPE(has_green, typename, bool, false);

		// true if the colour type has a blue (.b) channel.
		DEFAULTED_TEMPLATE_VALUE_TYPE(has_blue, typename, bool, false);

		// true if the colour type has an alpha (.a) channel.
		DEFAULTED_TEMPLATE_VALUE_TYPE(has_alpha, typename, bool, false);

		// true if the colour type has a value (.v) channel. may still be
		// trivially convertible to RGB and may have alpha.
		DEFAULTED_TEMPLATE_VALUE_TYPE(is_monochrome, typename, bool, false);

		// true if this colour type is intended to be used as an index into a
		// palette.
		DEFAULTED_TEMPLATE_VALUE_TYPE(is_indexed, typename, bool, false);

		// helper types for converting between different bit depths for a given
		// colour channel.
		struct channel0;
		struct channel1;
		struct channel5;
		struct channel6;
		struct channel8;

		template<size_t BIT_COUNT>
		struct channel_;
		template<size_t BIT_COUNT>
		using channel = typename channel_<BIT_COUNT>::type;

		// using channel0 = channel<0>;
		// using channel1 = channel<1>;
		// using channel5 = channel<5>;
		// using channel6 = channel<6>;
		// using channel8 = channel<8>;

		template<typename... T>
		struct common_channel;
		template<typename... T>
		using common_channel_t = typename common_channel<T...>::type;

#define LAK_FOREACH_CHANNEL_SIZE(MACRO, ...)                                  \
	MACRO(1, __VA_ARGS__)                                                       \
	MACRO(5, __VA_ARGS__)                                                       \
	MACRO(6, __VA_ARGS__)                                                       \
	MACRO(8, __VA_ARGS__)

#define LAK_FOREACH_CHANNEL_TYPE(MACRO, ...)                                  \
	MACRO(i, index, __VA_ARGS__)                                                \
	MACRO(a, alpha, __VA_ARGS__)                                                \
	MACRO(r, red, __VA_ARGS__)                                                  \
	MACRO(g, green, __VA_ARGS__)                                                \
	MACRO(b, blue, __VA_ARGS__)                                                 \
	MACRO(h, hue, __VA_ARGS__)                                                  \
	MACRO(s, saturation, __VA_ARGS__)                                           \
	MACRO(v, value, __VA_ARGS__)

#define LAK_FOREACH_CHANNEL_TYPE_EX(SIZE, MACRO, ...)                         \
	LAK_FOREACH_CHANNEL_TYPE(MACRO, SIZE, __VA_ARGS__)

// MACRO(SYMBOL, NAME, SIZE)
#define LAK_FOREACH_SIZED_CHANNEL_TYPE(MACRO, ...)                            \
	LAK_FOREACH_CHANNEL_SIZE(LAK_FOREACH_CHANNEL_TYPE_EX, MACRO, __VA_ARGS__)

		enum struct channel_type
		{
#define _LAK_DEF_CHANNEL_TYPE(SYMBOL, NAME, SIZE, ...) NAME##SIZE,
			LAK_FOREACH_SIZED_CHANNEL_TYPE(_LAK_DEF_CHANNEL_TYPE) last
#undef _LAK_DEF_CHANNEL_TYPE
		};

		TEMPLATE_VALUE_TYPE(bit_count, lak::colour::channel_type, size_t);

		TEMPLATE_VALUE_TYPE(channel_name, lak::colour::channel_type, const char *);

		// indexed
		struct i8; // 8bit

		// pure alpha
		struct a8; // 8bit

		// pure monochrome
		struct v8; // 8bit

		// alpha + monochrome
		struct av88; // 16bit
		struct va88; // 16bit

		// RGB
		struct rgb555; // 15bit
		struct rgb565; // 16bit
		struct rgb888; // 24bit

		// BGR
		struct bgr555;   // 15bit
		struct bgr565;   // 16bit
		struct bgr888;   // 24bit
		struct bgrx8888; // 32bit

		// RGBA
		struct rgba5551; // 16bit
		struct rgba5658; // 24bit
		struct rgba8888; // 32bit

		// ABGR
		struct abgr1555; // 16bit
		struct abgr8565; // 24bit
		struct abgr8888; // 32bit

		using bgrx5551 = bgr555;
		using rgbx5551 = rgb555;

		template<typename T, typename U>
		struct common_colour;
		template<typename T, typename U>
		using common_colour_t = typename common_colour<T, U>::type;

		template<
		  typename FROM,
		  typename TO,
		  lak::enable_if_i<lak::is_same_v<FROM, TO> && lak::is_colour_v<FROM>> = 0>
		void blit(lak::span<const FROM> from, lak::span<TO> to)
		{
			for (size_t i = 0, end = std::min(from.size(), to.size()); i < end; ++i)
				to[i] = from[i];
		}

		template<
		  typename FROM,
		  typename TO,
		  lak::enable_if_i<!lak::is_same_v<FROM, TO> && lak::is_colour_v<FROM> &&
		                   lak::is_colour_v<TO>> = 0>
		void blit(lak::span<const FROM> from, lak::span<TO> to)
		{
			for (size_t i = 0, end = std::min(from.size(), to.size()); i < end; ++i)
				to[i] = TO(from[i]);
		}

		template<typename LHS,
		         typename RHS,
		         typename ALPHA,
		         lak::enable_if_i<lak::is_colour_channel_v<LHS> &&
		                          lak::is_colour_channel_v<RHS> &&
		                          lak::is_colour_channel_v<ALPHA>> = 0>
		lak::colour::common_channel_t<LHS, RHS> lerp(const LHS &lhs,
		                                             const RHS &rhs,
		                                             const ALPHA &alpha)
		{
			using value_t = lak::colour::common_channel_t<LHS, RHS>;

			const auto lhsV = value_t(lhs).v;
			const auto rhsV = value_t(rhs).v;
			const auto alpV = alpha.v;

			return {((lhsV * (ALPHA::max_value - alpV)) + (rhsV * alpV)) /
			        ALPHA::max_value};
		}
	}

	template<typename COLOUR>
	constexpr lak::array<COLOUR, 256> xterm_palette()
	{
		static_assert(lak::is_colour_v<COLOUR>);

		lak::array<COLOUR, 256> result;
		auto palette = lak::span(result);

		uint8_t index = 0;
		for (auto &colour : palette.template first<7>())
		{
			colour.R((index & 0b001) * 0x80);
			colour.G((index & 0b010) * 0x80);
			colour.B((index & 0b100) * 0x80);
			++index;
		}

		palette[7].V(0xC0);
		palette[8].V(0x80);

		index = 0;
		for (auto &colour : palette.template subspan<9, 7>())
		{
			colour.R((index & 0b001) * 0xFF);
			colour.G((index & 0b010) * 0xFF);
			colour.B((index & 0b100) * 0xFF);
			++index;
		}

		index = 0;
		for (auto &colour : palette.template subspan<16, 216>())
		{
			const uint8_t r = index % 5;
			const uint8_t g = (index / 5) % 5;
			const uint8_t b = index / 10;

			colour.R(((r > 0) * 0x37) + (r * 0x28));
			colour.G(((g > 0) * 0x37) + (g * 0x28));
			colour.B(((b > 0) * 0x37) + (b * 0x28));
			++index;
		}

		index = 0;
		for (auto &colour : palette.template subspan<232, 24>())
		{
			colour.V((index * 10) + 8);
			++index;
		}

		return result;
	}
}

/* --- unsafe channel addition --- */

template<typename LHS,
         typename RHS,
         lak::enable_if_i<lak::is_colour_channel_v<LHS> &&
                          lak::is_colour_channel_v<RHS>> = 0>
lak::colour::common_channel_t<LHS, RHS> operator+(const LHS &lhs,
                                                  const RHS &rhs)
{
	using value_t = lak::colour::common_channel_t<LHS, RHS>;
	static_assert(value_t::max_value > 0);
	return {value_t(lhs).v + value_t(rhs).v};
}

/* --- unsafe channel subtraction --- */

template<typename LHS,
         typename RHS,
         lak::enable_if_i<lak::is_colour_channel_v<LHS> &&
                          lak::is_colour_channel_v<RHS>> = 0>
lak::colour::common_channel_t<LHS, RHS> operator-(const LHS &lhs,
                                                  const RHS &rhs)
{
	using value_t = lak::colour::common_channel_t<LHS, RHS>;
	static_assert(value_t::max_value > 0);
	return {value_t(lhs).v - value_t(rhs).v};
}

/* --- normalised channel multiplication --- */

template<typename LHS,
         typename RHS,
         lak::enable_if_i<lak::is_colour_channel_v<LHS> &&
                          lak::is_colour_channel_v<RHS>> = 0>
lak::colour::common_channel_t<LHS, RHS> operator*(const LHS &lhs,
                                                  const RHS &rhs)
{
	using value_t = lak::colour::common_channel_t<LHS, RHS>;
	static_assert(value_t::max_value > 0);
	return {(value_t(lhs).v * value_t(rhs).v) / value_t::max_value};
}

/* --- normalised channel division --- */

template<typename LHS,
         typename RHS,
         lak::enable_if_i<lak::is_colour_channel_v<LHS> &&
                          lak::is_colour_channel_v<RHS>> = 0>
lak::colour::common_channel_t<LHS, RHS> operator/(const LHS &lhs,
                                                  const RHS &rhs)
{
	using value_t = lak::colour::common_channel_t<LHS, RHS>;
	static_assert(value_t::max_value > 0);
	return {(value_t(lhs).v * value_t::max_value) / value_t(rhs).v};
}

/* --- colour addition --- */

template<typename LHS,
         typename RHS,
         lak::enable_if_i<lak::is_colour_v<LHS> && lak::is_colour_v<RHS>> = 0>
lak::colour::common_colour_t<LHS, RHS> operator+(const LHS &lhs,
                                                 const RHS &rhs)
{
	using result_t = lak::colour::common_colour_t<LHS, RHS>;
	result_t result;

	if constexpr (lak::colour::has_red_v<result_t>) result.R(lhs.R() + rhs.R());
	if constexpr (lak::colour::has_green_v<result_t>)
		result.G(lhs.G() + rhs.G());
	if constexpr (lak::colour::has_blue_v<result_t>) result.B(lhs.B() + rhs.B());
	if constexpr (lak::colour::has_alpha_v<result_t>)
		result.A(lhs.A() + rhs.A());
	if constexpr (lak::colour::is_monochrome_v<result_t>)
		result.V(lhs.V() + rhs.V());

	return result;
}

/* --- colour difference --- */

template<typename LHS,
         typename RHS,
         lak::enable_if_i<lak::is_colour_v<LHS> && lak::is_colour_v<RHS>> = 0>
lak::colour::common_colour_t<LHS, RHS> operator-(const LHS &lhs,
                                                 const RHS &rhs)
{
	using result_t = lak::colour::common_colour_t<LHS, RHS>;
	result_t result;

	if constexpr (lak::colour::has_red_v<result_t>) result.R(lhs.R() - rhs.R());
	if constexpr (lak::colour::has_green_v<result_t>)
		result.G(lhs.G() - rhs.G());
	if constexpr (lak::colour::has_blue_v<result_t>) result.B(lhs.B() - rhs.B());
	if constexpr (lak::colour::has_alpha_v<result_t>)
		result.A(lhs.A() - rhs.A());
	if constexpr (lak::colour::is_monochrome_v<result_t>)
		result.V(lhs.V() - rhs.V());

	return result;
}

/* --- colour blend --- */

template<typename LHS,
         typename RHS,
         lak::enable_if_i<lak::is_colour_v<LHS> && lak::is_colour_v<RHS>> = 0>
auto operator*(const LHS &lhs, const RHS &rhs)
{
	using result_t = lak::colour::common_colour_t<LHS, RHS>;
	result_t result;

	if constexpr (lak::colour::has_red_v<result_t>) result.R(lhs.R() * rhs.R());
	if constexpr (lak::colour::has_green_v<result_t>)
		result.G(lhs.G() * rhs.G());
	if constexpr (lak::colour::has_blue_v<result_t>) result.B(lhs.B() * rhs.B());
	if constexpr (lak::colour::has_alpha_v<result_t>)
		result.A(lhs.A() * rhs.A());
	if constexpr (lak::colour::is_monochrome_v<result_t>)
		result.V(lhs.V() * rhs.V());

	return result;
}

/* --- alpha blend --- */

//
// rhsA == 1 -> outRGBA = rhsRGBA
//
// lhsA == 1 -> outRGB = lerp(lhsRGB, rhsRGB, rhsA)
//
// outA = lerp(lhsA, 1.0, rhsA)
// outA == 0 -> outRGB = 0
// outA != 0 -> outRGB = lerp(lhsRGB * lhsA, rhsRGB, rhsA) / outA
//

template<typename LHS,
         typename RHS,
         lak::enable_if_i<lak::is_colour_v<LHS> && lak::is_colour_v<RHS>> = 0>
auto operator%(const LHS &lhs, const RHS &rhs)
{
	if constexpr (!lak::colour::has_alpha_v<RHS>)
	{
		// RHS doesn't have alpha (LHS is overwritten regardless of if it has
		// alpha).

		return rhs;
	}
	else if constexpr (!lak::colour::has_alpha_v<LHS>)
	{
		// RHS has alpha, LHS doesn't (compile time optimise for LHS alpha =
		// max_value).

		using result_t = lak::colour::common_colour_t<LHS, RHS>;

		result_t result;

		if constexpr (lak::colour::is_monochrome_v<result_t>)
			result.V(lak::colour::lerp(lhs.V(), rhs.V(), rhs.A()));

		if constexpr (lak::colour::has_red_v<result_t>)
			result.R(lak::colour::lerp(lhs.R(), rhs.R(), rhs.A()));

		if constexpr (lak::colour::has_green_v<result_t>)
			result.G(lak::colour::lerp(lhs.G(), rhs.G(), rhs.A()));

		if constexpr (lak::colour::has_blue_v<result_t>)
			result.B(lak::colour::lerp(lhs.B(), rhs.B(), rhs.A()));

		return result;
	}
	else
	{
		// both LHS and RHS have alpha.

		using result_t = lak::colour::common_colour_t<LHS, RHS>;
		static_assert(lak::colour::has_alpha_v<result_t>);

		result_t result;

		result.A(lak::colour::lerp(lhs.A(), rhs.A(), rhs.A()));

		if (result.a == 0)
		{
			if constexpr (lak::colour::has_red_v<result_t>) result.r = 0;
			if constexpr (lak::colour::has_green_v<result_t>) result.g = 0;
			if constexpr (lak::colour::has_blue_v<result_t>) result.b = 0;
			if constexpr (lak::colour::is_monochrome_v<result_t>) result.v = 0;
		}
		else
		{
			if constexpr (lak::colour::has_red_v<result_t>)
				result.R(lak::colour::lerp(lhs.R() * lhs.A(), rhs.R(), rhs.A()) /
				         result.A());

			if constexpr (lak::colour::has_green_v<result_t>)
				result.G(lak::colour::lerp(lhs.G() * lhs.A(), rhs.G(), rhs.A()) /
				         result.A());

			if constexpr (lak::colour::has_blue_v<result_t>)
				result.B(lak::colour::lerp(lhs.B() * lhs.A(), rhs.B(), rhs.A()) /
				         result.A());

			if constexpr (lak::colour::is_monochrome_v<result_t>)
				result.V(lak::colour::lerp(lhs.V() * lhs.A(), rhs.V(), rhs.A()) /
				         result.A());
		}

		return result;
	}
}

#define LAK_FOREACH_COLOUR(MACRO, ...)                                        \
	MACRO(a8, __VA_ARGS__)                                                      \
	MACRO(v8, __VA_ARGS__)                                                      \
	MACRO(av88, __VA_ARGS__)                                                    \
	MACRO(va88, __VA_ARGS__)                                                    \
	MACRO(rgb555, __VA_ARGS__)                                                  \
	MACRO(rgb565, __VA_ARGS__)                                                  \
	MACRO(rgb888, __VA_ARGS__)                                                  \
	MACRO(bgr555, __VA_ARGS__)                                                  \
	MACRO(bgr565, __VA_ARGS__)                                                  \
	MACRO(bgr888, __VA_ARGS__)                                                  \
	MACRO(bgrx8888, __VA_ARGS__)                                                \
	MACRO(rgba5551, __VA_ARGS__)                                                \
	MACRO(rgba5658, __VA_ARGS__)                                                \
	MACRO(rgba8888, __VA_ARGS__)                                                \
	MACRO(abgr1555, __VA_ARGS__)                                                \
	MACRO(abgr8565, __VA_ARGS__)                                                \
	MACRO(abgr8888, __VA_ARGS__)

#define LAK_FOREACH_COLOUR_PAIR_EX(COLOUR1, MACRO, ...)                       \
	LAK_FOREACH_COLOUR(MACRO, COLOUR1, __VA_ARGS__)

#define LAK_FOREACH_COLOUR_PAIR(MACRO, ...)                                   \
	LAK_FOREACH_COLOUR(LAK_FOREACH_COLOUR_PAIR_EX, MACRO, __VA_ARGS__)

#define LAK_FOREACH_VIRTUAL_COLOUR_CHANNEL(MACRO, ...)                        \
	MACRO(channel0, __VA_ARGS__)                                                \
	MACRO(channel1, __VA_ARGS__)                                                \
	MACRO(channel5, __VA_ARGS__)                                                \
	MACRO(channel6, __VA_ARGS__)                                                \
	MACRO(channel8, __VA_ARGS__)

#include "lak/colour.inl"

#endif
