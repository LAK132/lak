#ifndef LAK_VEC_HPP
#define LAK_VEC_HPP

#ifdef __has_include
#	if __has_include(<imgui.h>)
#		include <imgui.h>
#		define LAK_VEC_HPP_HAS_IMGUI
#	endif

#	if __has_include(<glm/vec2.hpp>) && \
       __has_include(<glm/vec3.hpp>) && \
       __has_include(<glm/vec4.hpp>)
#		include <glm/vec2.hpp>
#		include <glm/vec3.hpp>
#		include <glm/vec4.hpp>
#		define LAK_VEC_HPP_HAS_GLM
#	endif
#endif

#include "lak/binary_reader.hpp"
#include "lak/format.hpp"
#include "lak/stdint.hpp"

#include <cstddef>
#include <cstdint>
#include <tuple>

namespace lak
{
	template<typename T>
	struct vec2;
	template<typename T>
	struct vec3;
	template<typename T>
	struct vec4;

	template<typename T>
	struct vec2
	{
		union
		{
			T x;
			T r;
		};
		union
		{
			T y;
			T g;
		};

		vec2() : x(0), y(0) {}

		vec2(const vec2 &other) : x(other.x), y(other.y) {}

		vec2(const T (&values)[2]) : x(values[0]), y(values[1]) {}

		vec2(const T X, const T Y) : x(X), y(Y) {}

		vec2 &operator=(const vec2 &rhs)
		{
			x = (T)rhs.x;
			y = (T)rhs.y;
			return *this;
		}

		inline T &operator[](const size_t index) { return (&x)[index]; }

		inline const T &operator[](const size_t index) const
		{
			return (&x)[index];
		}

		template<size_t I>
		inline T &get()
		{
			if constexpr (I == 0)
				return x;
			else if constexpr (I == 1)
				return y;
		}

		template<size_t I>
		inline const T &get() const
		{
			if constexpr (I == 0)
				return x;
			else if constexpr (I == 1)
				return y;
		}

		template<typename L>
		explicit operator vec2<L>() const
		{
			return vec2<L>{static_cast<L>(x), static_cast<L>(y)};
		}

		template<typename L>
		explicit operator vec3<L>() const
		{
			return vec3<L>{static_cast<L>(x), static_cast<L>(y), 0};
		}

		template<typename L>
		explicit operator vec4<L>() const
		{
			return vec4<L>{static_cast<L>(x), static_cast<L>(y), 0, 0};
		}

#ifdef LAK_VEC_HPP_HAS_IMGUI
		explicit operator ImVec2() const
		{
			return ImVec2(static_cast<float>(x), static_cast<float>(y));
		}

		explicit operator ImVec4() const
		{
			return ImVec4(static_cast<float>(x), static_cast<float>(y), 0.0f, 0.0f);
		}
#endif

#ifdef LAK_VEC_HPP_HAS_GLM
		template<glm::precision GLMP>
		vec2(const glm::tvec2<T, GLMP> &vec2)
		{
			x = vec2.x;
			y = vec2.y;
		}

		template<glm::precision GLMP>
		vec2(const glm::tvec3<T, GLMP> &vec3)
		{
			x = vec3.x;
			y = vec3.y;
		}

		template<glm::precision GLMP>
		vec2(const glm::tvec4<T, GLMP> &vec4)
		{
			x = vec4.x;
			y = vec4.y;
		}

		template<glm::precision GLMP>
		operator glm::tvec2<T, GLMP>() const
		{
			return {x, y};
		}

		template<glm::precision GLMP>
		operator glm::tvec3<T, GLMP>() const
		{
			return {x, y, 0};
		}

		template<glm::precision GLMP>
		operator glm::tvec4<T, GLMP>() const
		{
			return {x, y, 0, 0};
		}

		template<typename GLMT, glm::precision GLMP>
		explicit vec2(const glm::tvec2<GLMT, GLMP> &vec2)
		{
			x = static_cast<T>(vec2.x);
			y = static_cast<T>(vec2.y);
		}

		template<typename GLMT, glm::precision GLMP>
		explicit vec2(const glm::tvec3<GLMT, GLMP> &vec3)
		{
			x = static_cast<T>(vec3.x);
			y = static_cast<T>(vec3.y);
		}

		template<typename GLMT, glm::precision GLMP>
		explicit vec2(const glm::tvec4<GLMT, GLMP> &vec4)
		{
			x = static_cast<T>(vec4.x);
			y = static_cast<T>(vec4.y);
		}

		template<typename GLMT, glm::precision GLMP>
		explicit operator glm::tvec4<GLMT, GLMP>() const
		{
			return {static_cast<GLMT>(x), static_cast<GLMT>(y), GLMT(0), GLMT(0)};
		}

		template<typename GLMT, glm::precision GLMP>
		explicit operator glm::tvec3<GLMT, GLMP>() const
		{
			return {static_cast<GLMT>(x), static_cast<GLMT>(y), GLMT(0)};
		}

		template<typename GLMT, glm::precision GLMP>
		explicit operator glm::tvec2<GLMT, GLMP>() const
		{
			return {static_cast<GLMT>(x), static_cast<GLMT>(y)};
		}
#endif
	};

	using vec2f_t   = vec2<float>;
	using vec2c_t   = vec2<signed char>;
	using vec2i_t   = vec2<int>;
	using vec2l_t   = vec2<long>;
	using vec2f32_t = vec2<f32_t>;
	using vec2f64_t = vec2<f64_t>;
	using vec2s_t   = vec2<size_t>;
	using vec2i8_t  = vec2<int8_t>;
	using vec2u8_t  = vec2<uint8_t>;
	using color2_t  = vec2<uint8_t>;
	using vec2i16_t = vec2<int16_t>;
	using vec2u16_t = vec2<uint16_t>;
	using vec2i32_t = vec2<int32_t>;
	using vec2u32_t = vec2<uint32_t>;
	using vec2i64_t = vec2<int64_t>;
	using vec2u64_t = vec2<uint64_t>;

	template<typename T>
	using mat2      = vec2<vec2<T>>;
	using mat2f_t   = mat2<float>;
	using mat2c_t   = mat2<signed char>;
	using mat2i_t   = mat2<int>;
	using mat2l_t   = mat2<long>;
	using mat2f32_t = mat2<f32_t>;
	using mat2f64_t = mat2<f64_t>;
	using mat2s_t   = mat2<size_t>;
	using mat2i8_t  = mat2<int8_t>;
	using mat2u8_t  = mat2<uint8_t>;
	using mat2i16_t = mat2<int16_t>;
	using mat2u16_t = mat2<uint16_t>;
	using mat2i32_t = mat2<int32_t>;
	using mat2u32_t = mat2<uint32_t>;
	using mat2i64_t = mat2<int64_t>;
	using mat2u64_t = mat2<uint64_t>;

	template<typename T>
	struct vec3
	{
		union
		{
			T x;
			T r;
		};
		union
		{
			T y;
			T g;
		};
		union
		{
			T z;
			T b;
		};

		vec3() : x(0), y(0), z(0) {}

		vec3(const vec3 &other) : x(other.x), y(other.y), z(other.z) {}

		vec3(const T (&values)[3]) : x(values[0]), y(values[1]), z(values[2]) {}

		vec3(const T X, const T Y, const T Z) : x(X), y(Y), z(Z) {}

		vec3 &operator=(const vec3 &rhs)
		{
			x = rhs.x;
			y = rhs.y;
			z = rhs.z;
			return *this;
		}

		inline T &operator[](const size_t index) { return (&x)[index]; }

		inline const T &operator[](const size_t index) const
		{
			return (&x)[index];
		}

		template<size_t I>
		inline T &get()
		{
			if constexpr (I == 0)
				return x;
			else if constexpr (I == 1)
				return y;
			else if constexpr (I == 2)
				return z;
		}

		template<size_t I>
		inline const T &get() const
		{
			if constexpr (I == 0)
				return x;
			else if constexpr (I == 1)
				return y;
			else if constexpr (I == 2)
				return z;
		}

		template<typename L>
		explicit operator vec2<L>() const
		{
			return vec2<L>{static_cast<L>(x), static_cast<L>(y)};
		}

		template<typename L>
		explicit operator vec3<L>() const
		{
			return vec3<L>{static_cast<L>(x), static_cast<L>(y), static_cast<L>(z)};
		}

		template<typename L>
		explicit operator vec4<L>() const
		{
			return vec4<L>{
			  static_cast<L>(x), static_cast<L>(y), static_cast<L>(z), 0};
		}

#ifdef LAK_VEC_HPP_HAS_IMGUI
		operator ImVec2() const
		{
			return ImVec2(static_cast<float>(x), static_cast<float>(y));
		}

		operator ImVec4() const
		{
			return ImVec4(static_cast<float>(x),
			              static_cast<float>(y),
			              static_cast<float>(z),
			              0.0f);
		}
#endif

#ifdef LAK_VEC_HPP_HAS_GLM
		template<glm::precision GLMP>
		vec3(const glm::tvec2<T, GLMP> &vec2)
		{
			x = vec2.x;
			y = vec2.y;
			z = 0;
		}

		template<glm::precision GLMP>
		vec3(const glm::tvec3<T, GLMP> &vec3)
		{
			x = vec3.x;
			y = vec3.y;
			z = vec3.z;
		}

		template<glm::precision GLMP>
		vec3(const glm::tvec4<T, GLMP> &vec4)
		{
			x = vec4.x;
			y = vec4.y;
			z = vec4.z;
		}

		template<glm::precision GLMP>
		operator glm::tvec2<T, GLMP>() const
		{
			return {x, y};
		}

		template<glm::precision GLMP>
		operator glm::tvec3<T, GLMP>() const
		{
			return {x, y, z};
		}

		template<glm::precision GLMP>
		operator glm::tvec4<T, GLMP>() const
		{
			return {x, y, z, 0};
		}

		template<typename GLMT, glm::precision GLMP>
		explicit vec3(const glm::tvec2<GLMT, GLMP> &vec2)
		{
			x = static_cast<T>(vec2.x);
			y = static_cast<T>(vec2.y);
			z = 0;
		}

		template<typename GLMT, glm::precision GLMP>
		explicit vec3(const glm::tvec3<GLMT, GLMP> &vec3)
		{
			x = static_cast<T>(vec3.x);
			y = static_cast<T>(vec3.y);
			z = static_cast<T>(vec3.z);
		}

		template<typename GLMT, glm::precision GLMP>
		explicit vec3(const glm::tvec4<GLMT, GLMP> &vec4)
		{
			x = static_cast<T>(vec4.x);
			y = static_cast<T>(vec4.y);
			z = static_cast<T>(vec4.z);
		}

		template<typename GLMT, glm::precision GLMP>
		explicit operator glm::tvec4<GLMT, GLMP>() const
		{
			return {static_cast<GLMT>(x),
			        static_cast<GLMT>(y),
			        static_cast<GLMT>(z),
			        GLMT(0)};
		}

		template<typename GLMT, glm::precision GLMP>
		explicit operator glm::tvec3<GLMT, GLMP>() const
		{
			return {
			  static_cast<GLMT>(x), static_cast<GLMT>(y), static_cast<GLMT>(z)};
		}

		template<typename GLMT, glm::precision GLMP>
		explicit operator glm::tvec2<GLMT, GLMP>() const
		{
			return {static_cast<GLMT>(x), static_cast<GLMT>(y)};
		}
#endif
	};

	using vec3f_t   = vec3<float>;
	using vec3c_t   = vec3<signed char>;
	using vec3i_t   = vec3<int>;
	using vec3l_t   = vec3<long>;
	using vec3f32_t = vec3<f32_t>;
	using vec3f64_t = vec3<f64_t>;
	using vec3s_t   = vec3<size_t>;
	using vec3i8_t  = vec3<int8_t>;
	using vec3u8_t  = vec3<uint8_t>;
	using color3_t  = vec3<uint8_t>;
	using vec3i16_t = vec3<int16_t>;
	using vec3u16_t = vec3<uint16_t>;
	using vec3i32_t = vec3<int32_t>;
	using vec3u32_t = vec3<uint32_t>;
	using vec3i64_t = vec3<int64_t>;
	using vec3u64_t = vec3<uint64_t>;

	template<typename T>
	using mat3      = vec3<vec3<T>>;
	using mat3f_t   = mat3<float>;
	using mat3c_t   = mat3<signed char>;
	using mat3i_t   = mat3<int>;
	using mat3l_t   = mat3<long>;
	using mat3f32_t = mat3<f32_t>;
	using mat3f64_t = mat3<f64_t>;
	using mat3s_t   = mat3<size_t>;
	using mat3i8_t  = mat3<int8_t>;
	using mat3u8_t  = mat3<uint8_t>;
	using mat3i16_t = mat3<int16_t>;
	using mat3u16_t = mat3<uint16_t>;
	using mat3i32_t = mat3<int32_t>;
	using mat3u32_t = mat3<uint32_t>;
	using mat3i64_t = mat3<int64_t>;
	using mat3u64_t = mat3<uint64_t>;

	template<typename T>
	struct vec4
	{
		union
		{
			T x;
			T r;
		};
		union
		{
			T y;
			T g;
		};
		union
		{
			T z;
			T b;
		};
		union
		{
			T w;
			T a;
		};

		vec4() : x(0), y(0), z(0), w(0) {}

		vec4(const vec4 &other) : x(other.x), y(other.y), z(other.z), w(other.w) {}

		vec4(const T (&values)[4])
		: x(values[0]), y(values[1]), z(values[2]), w(values[3])
		{
		}

		vec4(const T X, const T Y, const T Z, const T W) : x(X), y(Y), z(Z), w(W)
		{
		}

		vec4 &operator=(const vec4 &rhs)
		{
			x = rhs.x;
			y = rhs.y;
			z = rhs.z;
			w = rhs.w;
			return *this;
		}

		inline T &operator[](const size_t index) { return (&x)[index]; }

		inline const T &operator[](const size_t index) const
		{
			return (&x)[index];
		}

		template<size_t I>
		inline T &get()
		{
			if constexpr (I == 0)
				return x;
			else if constexpr (I == 1)
				return y;
			else if constexpr (I == 2)
				return z;
			else if constexpr (I == 3)
				return w;
		}

		template<size_t I>
		inline const T &get() const
		{
			if constexpr (I == 0)
				return x;
			else if constexpr (I == 1)
				return y;
			else if constexpr (I == 2)
				return z;
			else if constexpr (I == 3)
				return w;
		}

		template<typename L>
		explicit operator vec2<L>() const
		{
			return vec2<L>{static_cast<L>(x), static_cast<L>(y)};
		}

		template<typename L>
		explicit operator vec3<L>() const
		{
			return vec3<L>{static_cast<L>(x), static_cast<L>(y), static_cast<L>(z)};
		}

		template<typename L>
		explicit operator vec4<L>() const
		{
			return vec4<L>{static_cast<L>(x),
			               static_cast<L>(y),
			               static_cast<L>(z),
			               static_cast<L>(w)};
		}

#ifdef LAK_VEC_HPP_HAS_IMGUI
		operator ImVec2() const
		{
			return ImVec2(static_cast<float>(x), static_cast<float>(y));
		}

		operator ImVec4() const
		{
			return ImVec4(static_cast<float>(x),
			              static_cast<float>(y),
			              static_cast<float>(z),
			              static_cast<float>(w));
		}
#endif

#ifdef LAK_VEC_HPP_HAS_GLM
		template<glm::precision GLMP>
		vec4(const glm::tvec2<T, GLMP> &vec2)
		{
			x = vec2.x;
			y = vec2.y;
			z = 0;
			w = 0;
		}

		template<glm::precision GLMP>
		vec4(const glm::tvec3<T, GLMP> &vec3)
		{
			x = vec3.x;
			y = vec3.y;
			z = vec3.z;
			w = 0;
		}

		template<glm::precision GLMP>
		vec4(const glm::tvec4<T, GLMP> &vec4)
		{
			x = vec4.x;
			y = vec4.y;
			z = vec4.z;
			w = vec4.w;
		}

		template<glm::precision GLMP>
		operator glm::tvec2<T, GLMP>() const
		{
			return {x, y};
		}

		template<glm::precision GLMP>
		operator glm::tvec3<T, GLMP>() const
		{
			return {x, y, z};
		}

		template<glm::precision GLMP>
		operator glm::tvec4<T, GLMP>() const
		{
			return {x, y, z, w};
		}

		template<typename GLMT, glm::precision GLMP>
		explicit vec4(const glm::tvec2<GLMT, GLMP> &vec2)
		{
			x = static_cast<T>(vec2.x);
			y = static_cast<T>(vec2.y);
			z = 0;
			w = 0;
		}

		template<typename GLMT, glm::precision GLMP>
		explicit vec4(const glm::tvec3<GLMT, GLMP> &vec3)
		{
			x = static_cast<T>(vec3.x);
			y = static_cast<T>(vec3.y);
			z = static_cast<T>(vec3.z);
			w = 0;
		}

		template<typename GLMT, glm::precision GLMP>
		explicit vec4(const glm::tvec4<GLMT, GLMP> &vec4)
		{
			x = static_cast<T>(vec4.x);
			y = static_cast<T>(vec4.y);
			z = static_cast<T>(vec4.z);
			w = static_cast<T>(vec4.w);
		}

		template<typename GLMT, glm::precision GLMP>
		explicit operator glm::tvec4<GLMT, GLMP>() const
		{
			return {static_cast<GLMT>(x),
			        static_cast<GLMT>(y),
			        static_cast<GLMT>(z),
			        static_cast<GLMT>(w)};
		}

		template<typename GLMT, glm::precision GLMP>
		explicit operator glm::tvec3<GLMT, GLMP>() const
		{
			return {
			  static_cast<GLMT>(x), static_cast<GLMT>(y), static_cast<GLMT>(z)};
		}

		template<typename GLMT, glm::precision GLMP>
		explicit operator glm::tvec2<GLMT, GLMP>() const
		{
			return {static_cast<GLMT>(x), static_cast<GLMT>(y)};
		}
#endif
	};

	using vec4f_t   = vec4<float>;
	using vec4c_t   = vec4<signed char>;
	using vec4i_t   = vec4<int>;
	using vec4l_t   = vec4<long>;
	using vec4f32_t = vec4<f32_t>;
	using vec4f64_t = vec4<f64_t>;
	using vec4s_t   = vec4<size_t>;
	using vec4i8_t  = vec4<int8_t>;
	using vec4u8_t  = vec4<uint8_t>;
	using color4_t  = vec4<uint8_t>;
	using vec4i16_t = vec4<int16_t>;
	using vec4u16_t = vec4<uint16_t>;
	using vec4i32_t = vec4<int32_t>;
	using vec4u32_t = vec4<uint32_t>;
	using vec4i64_t = vec4<int64_t>;
	using vec4u64_t = vec4<uint64_t>;

	template<typename T>
	using mat4      = vec4<vec4<T>>;
	using mat4f_t   = mat4<float>;
	using mat4c_t   = mat4<signed char>;
	using mat4i_t   = mat4<int>;
	using mat4l_t   = mat4<long>;
	using mat4f32_t = mat4<f32_t>;
	using mat4f64_t = mat4<f64_t>;
	using mat4s_t   = mat4<size_t>;
	using mat4i8_t  = mat4<int8_t>;
	using mat4u8_t  = mat4<uint8_t>;
	using mat4i16_t = mat4<int16_t>;
	using mat4u16_t = mat4<uint16_t>;
	using mat4i32_t = mat4<int32_t>;
	using mat4u32_t = mat4<uint32_t>;
	using mat4i64_t = mat4<int64_t>;
	using mat4u64_t = mat4<uint64_t>;
}

namespace std
{
	template<typename T>
	struct tuple_size<lak::vec2<T>>
	{
		static constexpr size_t value = 2;
	};
	template<size_t I, typename T>
	struct tuple_element<I, lak::vec2<T>>
	{
		using type = T;
	};

	template<typename T>
	struct tuple_size<lak::vec3<T>>
	{
		static constexpr size_t value = 3;
	};
	template<size_t I, typename T>
	struct tuple_element<I, lak::vec3<T>>
	{
		using type = T;
	};

	template<typename T>
	struct tuple_size<lak::vec4<T>>
	{
		static constexpr size_t value = 4;
	};
	template<size_t I, typename T>
	struct tuple_element<I, lak::vec4<T>>
	{
		using type = T;
	};
}

// dot product

template<typename T>
T dot(const lak::vec2<T> &lhs, const lak::vec2<T> &rhs)
{
	return (lhs.x * rhs.x) + (lhs.y * rhs.y);
}

template<typename T>
T dot(const lak::vec3<T> &lhs, const lak::vec3<T> &rhs)
{
	return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z);
}

template<typename T>
T dot(const lak::vec4<T> &lhs, const lak::vec4<T> &rhs)
{
	return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z) + (lhs.w * rhs.w);
}

// hammard product

template<typename T>
lak::vec2<T> operator*(lak::vec2<T> lhs, const lak::vec2<T> &rhs)
{
	lhs.x *= rhs.x;
	lhs.y *= rhs.y;
	return lhs;
}

template<typename T>
lak::vec2<T> operator*(lak::vec2<T> lhs, T rhs)
{
	lhs.x *= rhs;
	lhs.y *= rhs;
	return lhs;
}

template<typename T>
lak::vec2<T> operator*(T lhs, lak::vec2<T> rhs)
{
	rhs.x *= lhs;
	rhs.y *= lhs;
	return rhs;
}

template<typename T>
lak::vec3<T> operator*(lak::vec3<T> lhs, const lak::vec3<T> &rhs)
{
	lhs.x *= rhs.x;
	lhs.y *= rhs.y;
	lhs.z *= rhs.z;
	return lhs;
}

template<typename T>
lak::vec3<T> operator*(lak::vec3<T> lhs, T rhs)
{
	lhs.x *= rhs;
	lhs.y *= rhs;
	lhs.z *= rhs;
	return lhs;
}

template<typename T>
lak::vec3<T> operator*(T lhs, lak::vec3<T> rhs)
{
	rhs.x *= lhs;
	rhs.y *= lhs;
	rhs.z *= lhs;
	return rhs;
}

template<typename T>
lak::vec4<T> operator*(lak::vec4<T> lhs, const lak::vec4<T> &rhs)
{
	lhs.x *= rhs.x;
	lhs.y *= rhs.y;
	lhs.z *= rhs.z;
	lhs.w *= rhs.w;
	return lhs;
}

template<typename T>
lak::vec4<T> operator*(lak::vec4<T> lhs, T rhs)
{
	lhs.x *= rhs;
	lhs.y *= rhs;
	lhs.z *= rhs;
	lhs.w *= rhs;
	return lhs;
}

template<typename T>
lak::vec4<T> operator*(T lhs, lak::vec4<T> rhs)
{
	rhs.x *= lhs;
	rhs.y *= lhs;
	rhs.z *= lhs;
	rhs.w *= lhs;
	return rhs;
}

template<typename T>
lak::vec2<T> &operator*=(lak::vec2<T> &lhs, const lak::vec2<T> &rhs)
{
	lhs.x *= rhs.x;
	lhs.y *= rhs.y;
	return lhs;
}

template<typename T>
lak::vec2<T> &operator*=(lak::vec2<T> &lhs, T rhs)
{
	lhs.x *= rhs;
	lhs.y *= rhs;
	return lhs;
}

template<typename T>
lak::vec3<T> &operator*=(lak::vec3<T> &lhs, const lak::vec3<T> &rhs)
{
	lhs.x *= rhs.x;
	lhs.y *= rhs.y;
	lhs.z *= rhs.z;
	return lhs;
}

template<typename T>
lak::vec3<T> &operator*=(lak::vec3<T> &lhs, T rhs)
{
	lhs.x *= rhs;
	lhs.y *= rhs;
	lhs.z *= rhs;
	return lhs;
}

template<typename T>
lak::vec4<T> &operator*=(lak::vec4<T> &lhs, const lak::vec4<T> &rhs)
{
	lhs.x *= rhs.x;
	lhs.y *= rhs.y;
	lhs.z *= rhs.z;
	lhs.w *= rhs.w;
	return lhs;
}

template<typename T>
lak::vec4<T> &operator*=(lak::vec4<T> &lhs, T rhs)
{
	lhs.x *= rhs;
	lhs.y *= rhs;
	lhs.z *= rhs;
	lhs.w *= rhs;
	return lhs;
}

// matrix multiplication

template<typename T>
lak::vec2<T> operator*(lak::mat2<T> mat, lak::vec2<T> vec)
{
	return {dot(mat.x, vec), dot(mat.y, vec)};
}

template<typename T>
lak::vec3<T> operator*(lak::mat3<T> mat, lak::vec3<T> vec)
{
	return {dot(mat.x, vec), dot(mat.y, vec), dot(mat.z, vec)};
}

template<typename T>
lak::vec4<T> operator*(lak::mat4<T> mat, lak::vec4<T> vec)
{
	return {dot(mat.x, vec), dot(mat.y, vec), dot(mat.z, vec), dot(mat.w, vec)};
}

template<typename T>
lak::vec2<T> &operator*=(lak::vec2<T> &vec, lak::mat2<T> mat)
{
	return vec = mat * vec;
}

template<typename T>
lak::vec3<T> &operator*=(lak::vec3<T> &vec, lak::mat3<T> mat)
{
	return vec = mat * vec;
}

template<typename T>
lak::vec4<T> &operator*=(lak::vec4<T> &vec, lak::mat4<T> mat)
{
	return vec = mat * vec;
}

// division

template<typename T>
lak::vec2<T> operator/(lak::vec2<T> lhs, T rhs)
{
	lhs.x /= rhs;
	lhs.y /= rhs;
	return lhs;
}

template<typename T>
lak::vec2<T> operator/(T lhs, lak::vec2<T> rhs)
{
	rhs.x = lhs / rhs.x;
	rhs.y = lhs / rhs.y;
	return rhs;
}

template<typename T>
lak::vec2<T> operator/(lak::vec2<T> lhs, lak::vec2<T> rhs)
{
	lhs.x /= rhs.x;
	lhs.y /= rhs.y;
	return lhs;
}

template<typename T>
lak::vec3<T> operator/(lak::vec3<T> lhs, T rhs)
{
	lhs.x /= rhs;
	lhs.y /= rhs;
	lhs.z /= rhs;
	return lhs;
}

template<typename T>
lak::vec3<T> operator/(T lhs, lak::vec3<T> rhs)
{
	rhs.x = lhs / rhs.x;
	rhs.y = lhs / rhs.y;
	rhs.z = lhs / rhs.z;
	return rhs;
}

template<typename T>
lak::vec3<T> operator/(lak::vec3<T> lhs, lak::vec3<T> rhs)
{
	lhs.x /= rhs.x;
	lhs.y /= rhs.y;
	lhs.z /= rhs.z;
	return lhs;
}

template<typename T>
lak::vec4<T> operator/(lak::vec4<T> lhs, T rhs)
{
	lhs.x /= rhs;
	lhs.y /= rhs;
	lhs.z /= rhs;
	lhs.w /= rhs;
	return lhs;
}

template<typename T>
lak::vec4<T> operator/(T lhs, lak::vec4<T> rhs)
{
	rhs.x = lhs / rhs.x;
	rhs.y = lhs / rhs.y;
	rhs.z = lhs / rhs.z;
	rhs.w = lhs / rhs.w;
	return rhs;
}

template<typename T>
lak::vec4<T> operator/(lak::vec4<T> lhs, lak::vec4<T> rhs)
{
	lhs.x /= rhs.x;
	lhs.y /= rhs.y;
	lhs.z /= rhs.z;
	lhs.w /= rhs.w;
	return lhs;
}

template<typename T>
lak::vec2<T> &operator/=(lak::vec2<T> &lhs, T rhs)
{
	lhs.x /= rhs;
	lhs.y /= rhs;
	return lhs;
}

template<typename T>
lak::vec2<T> &operator/=(lak::vec2<T> &lhs, lak::vec2<T> rhs)
{
	lhs.x /= rhs.x;
	lhs.y /= rhs.y;
	return lhs;
}

template<typename T>
lak::vec3<T> &operator/=(lak::vec3<T> &lhs, T rhs)
{
	lhs.x /= rhs;
	lhs.y /= rhs;
	lhs.z /= rhs;
	return lhs;
}

template<typename T>
lak::vec3<T> &operator/=(lak::vec3<T> &lhs, lak::vec3<T> rhs)
{
	lhs.x /= rhs.x;
	lhs.y /= rhs.y;
	lhs.z /= rhs.z;
	return lhs;
}

template<typename T>
lak::vec4<T> &operator/=(lak::vec4<T> &lhs, T rhs)
{
	lhs.x /= rhs;
	lhs.y /= rhs;
	lhs.z /= rhs;
	lhs.w /= rhs;
	return lhs;
}

template<typename T>
lak::vec4<T> &operator/=(lak::vec4<T> &lhs, lak::vec4<T> rhs)
{
	lhs.x /= rhs.x;
	lhs.y /= rhs.y;
	lhs.z /= rhs.z;
	lhs.w /= rhs.w;
	return lhs;
}

// addition

template<typename T>
lak::vec2<T> operator+(lak::vec2<T> lhs, const lak::vec2<T> &rhs)
{
	lhs.x += rhs.x;
	lhs.y += rhs.y;
	return lhs;
}

template<typename T>
lak::vec3<T> operator+(lak::vec3<T> lhs, const lak::vec3<T> &rhs)
{
	lhs.x += rhs.x;
	lhs.y += rhs.y;
	lhs.z += rhs.z;
	return lhs;
}

template<typename T>
lak::vec4<T> operator+(lak::vec4<T> lhs, const lak::vec4<T> &rhs)
{
	lhs.x += rhs.x;
	lhs.y += rhs.y;
	lhs.z += rhs.z;
	lhs.w += rhs.w;
	return lhs;
}

template<typename T>
lak::vec2<T> &operator+=(lak::vec2<T> &lhs, const lak::vec2<T> &rhs)
{
	lhs.x += rhs.x;
	lhs.y += rhs.y;
	return lhs;
}

template<typename T>
lak::vec3<T> &operator+=(lak::vec3<T> &lhs, const lak::vec3<T> &rhs)
{
	lhs.x += rhs.x;
	lhs.y += rhs.y;
	lhs.z += rhs.z;
	return lhs;
}

template<typename T>
lak::vec4<T> &operator+=(lak::vec4<T> &lhs, const lak::vec4<T> &rhs)
{
	lhs.x += rhs.x;
	lhs.y += rhs.y;
	lhs.z += rhs.z;
	lhs.w += rhs.w;
	return lhs;
}

// subtraction

template<typename T>
lak::vec2<T> operator-(lak::vec2<T> lhs, const lak::vec2<T> &rhs)
{
	lhs.x -= rhs.x;
	lhs.y -= rhs.y;
	return lhs;
}

template<typename T>
lak::vec3<T> operator-(lak::vec3<T> lhs, const lak::vec3<T> &rhs)
{
	lhs.x -= rhs.x;
	lhs.y -= rhs.y;
	lhs.z -= rhs.z;
	return lhs;
}

template<typename T>
lak::vec4<T> operator-(lak::vec4<T> lhs, const lak::vec4<T> &rhs)
{
	lhs.x -= rhs.x;
	lhs.y -= rhs.y;
	lhs.z -= rhs.z;
	lhs.w -= rhs.w;
	return lhs;
}

template<typename T>
lak::vec2<T> &operator-=(lak::vec2<T> &lhs, const lak::vec2<T> &rhs)
{
	lhs.x -= rhs.x;
	lhs.y -= rhs.y;
	return lhs;
}

template<typename T>
lak::vec3<T> &operator-=(lak::vec3<T> &lhs, const lak::vec3<T> &rhs)
{
	lhs.x -= rhs.x;
	lhs.y -= rhs.y;
	lhs.z -= rhs.z;
	return lhs;
}

template<typename T>
lak::vec4<T> &operator-=(lak::vec4<T> &lhs, const lak::vec4<T> &rhs)
{
	lhs.x -= rhs.x;
	lhs.y -= rhs.y;
	lhs.z -= rhs.z;
	lhs.w -= rhs.w;
	return lhs;
}

template<typename T>
bool operator==(const lak::vec2<T> &lhs, const lak::vec2<T> &rhs)
{
	return (lhs.x == rhs.x) && (lhs.y == rhs.y);
}

template<typename T>
bool operator==(const lak::vec3<T> &lhs, const lak::vec3<T> &rhs)
{
	return (lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z);
}

template<typename T>
bool operator==(const lak::vec4<T> &lhs, const lak::vec4<T> &rhs)
{
	return (lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z) &&
	       (lhs.w == rhs.w);
}

template<typename T>
bool operator!=(const lak::vec2<T> &lhs, const lak::vec2<T> &rhs)
{
	return !(lhs == rhs);
}

template<typename T>
bool operator!=(const lak::vec3<T> &lhs, const lak::vec3<T> &rhs)
{
	return !(lhs == rhs);
}

template<typename T>
bool operator!=(const lak::vec4<T> &lhs, const lak::vec4<T> &rhs)
{
	return !(lhs == rhs);
}

template<typename T, typename CHAR>
struct lak::format_traits<lak::vec2<T>, CHAR>
{
	static_assert(lak::concepts::formattable<T>);

	using format_args = lak::format_args_t<T, CHAR>;

	static consteval format_args parse_args(lak::string_view<CHAR> fmt)
	{
		return lak::parse_format_args<T>(fmt);
	}

	static constexpr lak::string<CHAR> to_string(const format_args &args,
	                                             const lak::vec2<T> &val)
	requires(lak::concepts::dynamic_formattable<T, CHAR>)
	{
		return lak::fmt<CHAR, "({} {})">(
		  lak::format_traits<T, CHAR>::to_string(args, val.x),
		  lak::format_traits<T, CHAR>::to_string(args, val.y));
	}

	static constexpr lak::string<CHAR> to_string(const lak::vec2<T> &val)
	requires(!lak::concepts::dynamic_formattable<T, CHAR>)
	{
		return lak::fmt<CHAR, "({} {})">(val.x, val.y);
	}
};

template<typename T, typename CHAR>
struct lak::format_traits<lak::vec3<T>, CHAR>
{
	static_assert(lak::concepts::formattable<T>);

	using format_args = lak::format_args_t<T, CHAR>;

	static consteval format_args parse_args(lak::string_view<CHAR> fmt)
	{
		return lak::parse_format_args<T>(fmt);
	}

	static constexpr lak::string<CHAR> to_string(const format_args &args,
	                                             const lak::vec3<T> &val)
	requires(lak::concepts::dynamic_formattable<T, CHAR>)
	{
		return lak::fmt<CHAR, "({} {} {})">(
		  lak::format_traits<T, CHAR>::to_string(args, val.x),
		  lak::format_traits<T, CHAR>::to_string(args, val.y),
		  lak::format_traits<T, CHAR>::to_string(args, val.z));
	}

	static constexpr lak::string<CHAR> to_string(const lak::vec3<T> &val)
	requires(!lak::concepts::dynamic_formattable<T, CHAR>)
	{
		return lak::fmt<CHAR, "({} {} {})">(val.x, val.y, val.z);
	}
};

template<typename T, typename CHAR>
struct lak::format_traits<lak::vec4<T>, CHAR>
{
	static_assert(lak::concepts::formattable<T>);

	using format_args = lak::format_args_t<T, CHAR>;

	static consteval format_args parse_args(lak::string_view<CHAR> fmt)
	{
		return lak::parse_format_args<T>(fmt);
	}

	static constexpr lak::string<CHAR> to_string(const format_args &args,
	                                             const lak::vec4<T> &val)
	requires(lak::concepts::dynamic_formattable<T, CHAR>)
	{
		return lak::fmt<CHAR, "({} {} {} {})">(
		  lak::format_traits<T, CHAR>::to_string(args, val.x),
		  lak::format_traits<T, CHAR>::to_string(args, val.y),
		  lak::format_traits<T, CHAR>::to_string(args, val.z),
		  lak::format_traits<T, CHAR>::to_string(args, val.w));
	}

	static constexpr lak::string<CHAR> to_string(const lak::vec4<T> &val)
	requires(!lak::concepts::dynamic_formattable<T, CHAR>)
	{
		return lak::fmt<CHAR, "({} {} {} {})">(val.x, val.y, val.z, val.w);
	}
};

LAK_FIXED_TEMPLATE_STRUCT_BYTES_TRAITS(typename T,
                                       lak::vec2<T>,
                                       &lak::vec2<T>::x,
                                       &lak::vec2<T>::y);

LAK_FIXED_TEMPLATE_STRUCT_BYTES_TRAITS(typename T,
                                       lak::vec3<T>,
                                       &lak::vec3<T>::x,
                                       &lak::vec3<T>::y,
                                       &lak::vec3<T>::z);

LAK_FIXED_TEMPLATE_STRUCT_BYTES_TRAITS(typename T,
                                       lak::vec4<T>,
                                       &lak::vec4<T>::x,
                                       &lak::vec4<T>::y,
                                       &lak::vec4<T>::z,
                                       &lak::vec4<T>::w);

#endif
