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
	template<typename T, size_t S>
	struct vec;

	template<typename T>
	struct vec<T, 2U>
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

		constexpr vec() : x(0), y(0) {}

		constexpr vec(const T v) : x(v), y(v) {}

		constexpr vec(const vec &other) : x(other.x), y(other.y) {}

		constexpr vec(const T (&values)[2U]) : x(values[0U]), y(values[1U]) {}

		constexpr vec(const T X, const T Y) : x(X), y(Y) {}

		constexpr vec &operator=(const vec &rhs)
		{
			x = (T)rhs.x;
			y = (T)rhs.y;
			return *this;
		}

		constexpr T &operator[](const size_t index)
		{
			if (std::is_constant_evaluated())
			{
				if (index == 0U)
					return x;
				else if (index == 1U)
					return y;
				else
					ASSERT_UNREACHABLE();
			}
			else
			{
				return (&x)[index];
			}
		}

		constexpr const T &operator[](const size_t index) const
		{
			if (std::is_constant_evaluated())
			{
				if (index == 0U)
					return x;
				else if (index == 1U)
					return y;
				else
					ASSERT_UNREACHABLE();
			}
			else
			{
				return (&x)[index];
			}
		}

		template<size_t I>
		requires((I < 2))
		constexpr T &get()
		{
			if constexpr (I == 0)
				return x;
			else if constexpr (I == 1)
				return y;
		}

		template<size_t I>
		requires((I < 2))
		constexpr const T &get() const
		{
			if constexpr (I == 0)
				return x;
			else if constexpr (I == 1)
				return y;
		}

		template<typename L>
		constexpr explicit operator vec<L, 2U>() const
		{
			return vec<L, 2U>{static_cast<L>(x), static_cast<L>(y)};
		}

		template<typename L>
		constexpr explicit operator vec<L, 3U>() const
		{
			return vec<L, 3U>{static_cast<L>(x), static_cast<L>(y), L(0)};
		}

		template<typename L>
		constexpr explicit operator vec<L, 4U>() const
		{
			return vec<L, 4U>{static_cast<L>(x), static_cast<L>(y), L(0), L(0)};
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
		vec(const glm::tvec2<T, GLMP> &vec2)
		{
			x = vec2.x;
			y = vec2.y;
		}

		template<glm::precision GLMP>
		vec(const glm::tvec3<T, GLMP> &vec3)
		{
			x = vec3.x;
			y = vec3.y;
		}

		template<glm::precision GLMP>
		vec(const glm::tvec4<T, GLMP> &vec4)
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
		explicit vec(const glm::tvec2<GLMT, GLMP> &vec2)
		{
			x = static_cast<T>(vec2.x);
			y = static_cast<T>(vec2.y);
		}

		template<typename GLMT, glm::precision GLMP>
		explicit vec(const glm::tvec3<GLMT, GLMP> &vec3)
		{
			x = static_cast<T>(vec3.x);
			y = static_cast<T>(vec3.y);
		}

		template<typename GLMT, glm::precision GLMP>
		explicit vec(const glm::tvec4<GLMT, GLMP> &vec4)
		{
			x = static_cast<T>(vec4.x);
			y = static_cast<T>(vec4.y);
		}

		template<typename GLMT, glm::precision GLMP>
		explicit operator glm::tvec2<GLMT, GLMP>() const
		{
			return {static_cast<GLMT>(x), static_cast<GLMT>(y)};
		}

		template<typename GLMT, glm::precision GLMP>
		explicit operator glm::tvec3<GLMT, GLMP>() const
		{
			return {static_cast<GLMT>(x), static_cast<GLMT>(y), GLMT(0)};
		}

		template<typename GLMT, glm::precision GLMP>
		explicit operator glm::tvec4<GLMT, GLMP>() const
		{
			return {static_cast<GLMT>(x), static_cast<GLMT>(y), GLMT(0), GLMT(0)};
		}
#endif
	};

	template<typename T>
	using vec2 = lak::vec<T, 2u>;

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
	struct vec<T, 3U>
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

		constexpr vec() : x(0), y(0), z(0) {}

		constexpr vec(const T v) : x(v), y(v), z(v) {}

		constexpr vec(const vec &other) : x(other.x), y(other.y), z(other.z) {}

		constexpr vec(const lak::vec<T, 2U> &other, const T Z)
		: x(other.x), y(other.y), z(Z)
		{
		}

		constexpr vec(const T (&values)[3U])
		: x(values[0U]), y(values[1U]), z(values[2U])
		{
		}

		constexpr vec(const T X, const T Y, const T Z) : x(X), y(Y), z(Z) {}

		constexpr vec &operator=(const vec &rhs)
		{
			x = rhs.x;
			y = rhs.y;
			z = rhs.z;
			return *this;
		}

		constexpr T &operator[](const size_t index)
		{
			if (std::is_constant_evaluated())
			{
				if (index == 0U)
					return x;
				else if (index == 1U)
					return y;
				else if (index == 2U)
					return z;
				else
					ASSERT_UNREACHABLE();
			}
			else
			{
				return (&x)[index];
			}
		}

		constexpr const T &operator[](const size_t index) const
		{
			if (std::is_constant_evaluated())
			{
				if (index == 0U)
					return x;
				else if (index == 1U)
					return y;
				else if (index == 2U)
					return z;
				else
					ASSERT_UNREACHABLE();
			}
			else
			{
				return (&x)[index];
			}
		}

		template<size_t I>
		requires((I < 3))
		constexpr T &get()
		{
			if constexpr (I == 0)
				return x;
			else if constexpr (I == 1)
				return y;
			else if constexpr (I == 2)
				return z;
		}

		template<size_t I>
		requires((I < 3))
		constexpr const T &get() const
		{
			if constexpr (I == 0)
				return x;
			else if constexpr (I == 1)
				return y;
			else if constexpr (I == 2)
				return z;
		}

		template<typename L>
		constexpr explicit operator vec<L, 2U>() const
		{
			return vec<L, 2U>{static_cast<L>(x), static_cast<L>(y)};
		}

		template<typename L>
		constexpr explicit operator vec<L, 3U>() const
		{
			return vec<L, 3U>{
			  static_cast<L>(x), static_cast<L>(y), static_cast<L>(z)};
		}

		template<typename L>
		constexpr explicit operator vec<L, 4U>() const
		{
			return vec<L, 4U>{
			  static_cast<L>(x), static_cast<L>(y), static_cast<L>(z), L(0)};
		}

		constexpr lak::vec<T, 2U> xy() const { return {x, y}; }
		constexpr lak::vec<T, 2U> xz() const { return {x, z}; }
		constexpr lak::vec<T, 2U> yz() const { return {y, z}; }
		constexpr lak::vec<T, 2U> yx() const { return {y, x}; }
		constexpr lak::vec<T, 2U> zx() const { return {z, x}; }
		constexpr lak::vec<T, 2U> zy() const { return {z, y}; }

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
		vec(const glm::tvec2<T, GLMP> &vec2)
		{
			x = vec2.x;
			y = vec2.y;
			z = 0;
		}

		template<glm::precision GLMP>
		vec(const glm::tvec3<T, GLMP> &vec3)
		{
			x = vec3.x;
			y = vec3.y;
			z = vec3.z;
		}

		template<glm::precision GLMP>
		vec(const glm::tvec4<T, GLMP> &vec4)
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
		explicit vec(const glm::tvec2<GLMT, GLMP> &vec2)
		{
			x = static_cast<T>(vec2.x);
			y = static_cast<T>(vec2.y);
			z = 0;
		}

		template<typename GLMT, glm::precision GLMP>
		explicit vec(const glm::tvec3<GLMT, GLMP> &vec3)
		{
			x = static_cast<T>(vec3.x);
			y = static_cast<T>(vec3.y);
			z = static_cast<T>(vec3.z);
		}

		template<typename GLMT, glm::precision GLMP>
		explicit vec(const glm::tvec4<GLMT, GLMP> &vec4)
		{
			x = static_cast<T>(vec4.x);
			y = static_cast<T>(vec4.y);
			z = static_cast<T>(vec4.z);
		}

		template<typename GLMT, glm::precision GLMP>
		explicit operator glm::tvec2<GLMT, GLMP>() const
		{
			return {static_cast<GLMT>(x), static_cast<GLMT>(y)};
		}

		template<typename GLMT, glm::precision GLMP>
		explicit operator glm::tvec3<GLMT, GLMP>() const
		{
			return {
			  static_cast<GLMT>(x), static_cast<GLMT>(y), static_cast<GLMT>(z)};
		}

		template<typename GLMT, glm::precision GLMP>
		explicit operator glm::tvec4<GLMT, GLMP>() const
		{
			return {static_cast<GLMT>(x),
			        static_cast<GLMT>(y),
			        static_cast<GLMT>(z),
			        GLMT(0)};
		}
#endif
	};

	template<typename T>
	using vec3 = lak::vec<T, 3u>;

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
	struct vec<T, 4U>
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

		constexpr vec() : x(0), y(0), z(0), w(0) {}

		constexpr vec(const T v) : x(v), y(v), z(v), w(v) {}

		constexpr vec(const vec &other)
		: x(other.x), y(other.y), z(other.z), w(other.w)
		{
		}

		constexpr vec(const lak::vec<T, 2U> &other, const T Z, const T W)
		: x(other.x), y(other.y), z(Z), w(W)
		{
		}

		constexpr vec(const lak::vec<T, 3U> &other, const T W)
		: x(other.x), y(other.y), z(other.z), w(W)
		{
		}

		constexpr vec(const T (&values)[4U])
		: x(values[0U]), y(values[1U]), z(values[2U]), w(values[3U])
		{
		}

		constexpr vec(const T X, const T Y, const T Z, const T W)
		: x(X), y(Y), z(Z), w(W)
		{
		}

		constexpr vec &operator=(const vec &rhs)
		{
			x = rhs.x;
			y = rhs.y;
			z = rhs.z;
			w = rhs.w;
			return *this;
		}

		constexpr T &operator[](const size_t index)
		{
			if (std::is_constant_evaluated())
			{
				if (index == 0U)
					return x;
				else if (index == 1U)
					return y;
				else if (index == 2U)
					return z;
				else if (index == 3U)
					return w;
				else
					ASSERT_UNREACHABLE();
			}
			else
			{
				return (&x)[index];
			}
		}

		constexpr const T &operator[](const size_t index) const
		{
			if (std::is_constant_evaluated())
			{
				if (index == 0U)
					return x;
				else if (index == 1U)
					return y;
				else if (index == 2U)
					return z;
				else if (index == 3U)
					return w;
				else
					ASSERT_UNREACHABLE();
			}
			else
			{
				return (&x)[index];
			}
		}

		template<size_t I>
		requires((I < 4))
		constexpr T &get()
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
		requires((I < 4))
		constexpr const T &get() const
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
		constexpr explicit operator vec<L, 2U>() const
		{
			return vec<L, 2U>{static_cast<L>(x), static_cast<L>(y)};
		}

		template<typename L>
		constexpr explicit operator vec<L, 3U>() const
		{
			return vec<L, 3U>{
			  static_cast<L>(x), static_cast<L>(y), static_cast<L>(z)};
		}

		template<typename L>
		constexpr explicit operator vec<L, 4U>() const
		{
			return vec<L, 4U>{static_cast<L>(x),
			                  static_cast<L>(y),
			                  static_cast<L>(z),
			                  static_cast<L>(w)};
		}

		constexpr lak::vec<T, 2U> xy() const { return {x, y}; }
		constexpr lak::vec<T, 2U> xz() const { return {x, z}; }
		constexpr lak::vec<T, 2U> xw() const { return {x, w}; }
		constexpr lak::vec<T, 2U> yz() const { return {y, z}; }
		constexpr lak::vec<T, 2U> yw() const { return {y, w}; }
		constexpr lak::vec<T, 2U> zw() const { return {z, w}; }
		constexpr lak::vec<T, 2U> zx() const { return {z, x}; }
		constexpr lak::vec<T, 2U> wx() const { return {w, x}; }
		constexpr lak::vec<T, 2U> wy() const { return {w, y}; }

		constexpr lak::vec<T, 3U> xyz() const { return {x, y, z}; }
		constexpr lak::vec<T, 3U> xyw() const { return {x, y, w}; }
		constexpr lak::vec<T, 3U> xzw() const { return {x, z, w}; }
		constexpr lak::vec<T, 3U> yzw() const { return {y, z, w}; }
		constexpr lak::vec<T, 3U> zwx() const { return {z, w, x}; }
		constexpr lak::vec<T, 3U> wxy() const { return {w, x, y}; }

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
		vec(const glm::tvec2<T, GLMP> &vec2)
		{
			x = vec2.x;
			y = vec2.y;
			z = 0;
			w = 0;
		}

		template<glm::precision GLMP>
		vec(const glm::tvec3<T, GLMP> &vec3)
		{
			x = vec3.x;
			y = vec3.y;
			z = vec3.z;
			w = 0;
		}

		template<glm::precision GLMP>
		vec(const glm::tvec4<T, GLMP> &vec4)
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
		explicit vec(const glm::tvec2<GLMT, GLMP> &vec2)
		{
			x = static_cast<T>(vec2.x);
			y = static_cast<T>(vec2.y);
			z = 0;
			w = 0;
		}

		template<typename GLMT, glm::precision GLMP>
		explicit vec(const glm::tvec3<GLMT, GLMP> &vec3)
		{
			x = static_cast<T>(vec3.x);
			y = static_cast<T>(vec3.y);
			z = static_cast<T>(vec3.z);
			w = 0;
		}

		template<typename GLMT, glm::precision GLMP>
		explicit vec(const glm::tvec4<GLMT, GLMP> &vec4)
		{
			x = static_cast<T>(vec4.x);
			y = static_cast<T>(vec4.y);
			z = static_cast<T>(vec4.z);
			w = static_cast<T>(vec4.w);
		}

		template<typename GLMT, glm::precision GLMP>
		explicit operator glm::tvec2<GLMT, GLMP>() const
		{
			return {static_cast<GLMT>(x), static_cast<GLMT>(y)};
		}

		template<typename GLMT, glm::precision GLMP>
		explicit operator glm::tvec3<GLMT, GLMP>() const
		{
			return {
			  static_cast<GLMT>(x), static_cast<GLMT>(y), static_cast<GLMT>(z)};
		}

		template<typename GLMT, glm::precision GLMP>
		explicit operator glm::tvec4<GLMT, GLMP>() const
		{
			return {static_cast<GLMT>(x),
			        static_cast<GLMT>(y),
			        static_cast<GLMT>(z),
			        static_cast<GLMT>(w)};
		}
#endif
	};

	template<typename T>
	using vec4 = lak::vec<T, 4u>;

	template<typename T>
	struct is_vec : lak::false_type
	{
	};
	template<typename T, size_t S>
	struct is_vec<lak::vec<T, S>> : lak::true_type
	{
	};
	template<typename T>
	constexpr bool is_vec_v = lak::is_vec<T>::value;
	namespace concepts
	{
		template<typename T>
		concept vec = lak::is_vec_v<T>;
	}

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

	template<typename T, size_t R, size_t C>
	using mat = lak::vec<lak::vec<T, C>, R>;

	template<typename T>
	struct is_mat : lak::false_type
	{
	};
	template<typename T, size_t R, size_t C>
	struct is_mat<lak::mat<T, R, C>> : lak::true_type
	{
	};
	template<typename T>
	constexpr bool is_mat_v = lak::is_mat<T>::value;
	namespace concepts
	{
		template<typename T>
		concept mat = lak::is_mat_v<T>;
	}

	template<typename T>
	using mat2      = mat<T, 2U, 2U>;
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
	using mat3      = mat<T, 3U, 3U>;
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
	using mat4      = mat<T, 4U, 4U>;
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
	template<typename T, size_t S>
	struct tuple_size<lak::vec<T, S>>
	{
		static constexpr size_t value = S;
	};
	template<size_t I, typename T, size_t S>
	struct tuple_element<I, lak::vec<T, S>>
	{
		using type = T;
	};
}

namespace lak
{
	template<typename T, size_t S>
	constexpr lak::mat<T, S, S> diagonal(lak::vec<T, S> v)
	{
		lak::mat<T, S, S> result{0.f};
		for (size_t i = 0U; i < S; ++i) result[i][i] = v[i];
		return result;
	}

	template<typename T>
	constexpr lak::vec3<T> homogenise(lak::vec2<T> v)
	{
		return {v, 1.f};
	}

	template<typename T>
	constexpr lak::vec4<T> homogenise(lak::vec3<T> v)
	{
		return {v, 1.f};
	}

	template<typename T>
	constexpr lak::vec2<T> dehomogenise(lak::vec3<T> v)
	{
		return {v.x / v.z, v.y / v.z};
	}

	template<typename T>
	constexpr lak::vec3<T> dehomogenise(lak::vec4<T> v)
	{
		return {v.x / v.w, v.y / v.w, v.z / v.w};
	}

	// dot product

	template<typename T>
	constexpr T dot(const lak::vec2<T> &lhs, const lak::vec2<T> &rhs)
	{
		return (lhs.x * rhs.x) + (lhs.y * rhs.y);
	}

	template<typename T>
	constexpr T dot(const lak::vec3<T> &lhs, const lak::vec3<T> &rhs)
	{
		return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z);
	}

	template<typename T>
	constexpr T dot(const lak::vec4<T> &lhs, const lak::vec4<T> &rhs)
	{
		return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z) +
		       (lhs.w * rhs.w);
	}

	// mag

	template<typename T, size_t S>
	constexpr T mag2(const lak::vec<T, S> &vec)
	{
		return lak::dot(vec, vec);
	}

	template<typename T, size_t S>
	constexpr T mag(const lak::vec<T, S> &vec)
	{
		return std::sqrt(mag2(vec));
	}
}

template<typename T, size_t S>
constexpr T operator~(const lak::vec<T, S> &vec)
{
	return lak::mag(vec);
}

namespace lak
{
	// manhattan distance

	template<typename T>
	constexpr T manhattan(const lak::vec2<T> &vec)
	{
		return vec.x + vec.y;
	}

	template<typename T>
	constexpr T manhattan(const lak::vec3<T> &vec)
	{
		return vec.x + vec.y + vec.z;
	}

	template<typename T>
	constexpr T manhattan(const lak::vec4<T> &vec)
	{
		return vec.x + vec.y + vec.z + vec.w;
	}

	template<typename T>
	constexpr T abs_manhattan(const lak::vec2<T> &vec)
	{
		return std::abs(vec.x) + std::abs(vec.y);
	}

	template<typename T>
	constexpr T abs_manhattan(const lak::vec3<T> &vec)
	{
		return std::abs(vec.x) + std::abs(vec.y) + std::abs(vec.z);
	}

	template<typename T>
	constexpr T abs_manhattan(const lak::vec4<T> &vec)
	{
		return std::abs(vec.x) + std::abs(vec.y) + std::abs(vec.z) +
		       std::abs(vec.w);
	}
}

// hammard product

template<typename T>
constexpr lak::vec2<T> operator*(lak::vec2<T> lhs, const lak::vec2<T> &rhs)
{
	lhs.x *= rhs.x;
	lhs.y *= rhs.y;
	return lhs;
}

template<typename T>
requires(!lak::is_vec_v<T>)
constexpr lak::vec2<T> operator*(lak::vec2<T> lhs, T rhs)
{
	lhs.x *= rhs;
	lhs.y *= rhs;
	return lhs;
}

template<typename T>
requires(!lak::is_vec_v<T>)
constexpr lak::vec2<T> operator*(T lhs, lak::vec2<T> rhs)
{
	rhs.x *= lhs;
	rhs.y *= lhs;
	return rhs;
}

template<typename T>
constexpr lak::vec3<T> operator*(lak::vec3<T> lhs, const lak::vec3<T> &rhs)
{
	lhs.x *= rhs.x;
	lhs.y *= rhs.y;
	lhs.z *= rhs.z;
	return lhs;
}

template<typename T>
requires(!lak::is_vec_v<T>)
constexpr lak::vec3<T> operator*(lak::vec3<T> lhs, T rhs)
{
	lhs.x *= rhs;
	lhs.y *= rhs;
	lhs.z *= rhs;
	return lhs;
}

template<typename T>
requires(!lak::is_vec_v<T>)
constexpr lak::vec3<T> operator*(T lhs, lak::vec3<T> rhs)
{
	rhs.x *= lhs;
	rhs.y *= lhs;
	rhs.z *= lhs;
	return rhs;
}

template<typename T>
constexpr lak::vec4<T> operator*(lak::vec4<T> lhs, const lak::vec4<T> &rhs)
{
	lhs.x *= rhs.x;
	lhs.y *= rhs.y;
	lhs.z *= rhs.z;
	lhs.w *= rhs.w;
	return lhs;
}

template<typename T>
requires(!lak::is_vec_v<T>)
constexpr lak::vec4<T> operator*(lak::vec4<T> lhs, T rhs)
{
	lhs.x *= rhs;
	lhs.y *= rhs;
	lhs.z *= rhs;
	lhs.w *= rhs;
	return lhs;
}

template<typename T>
requires(!lak::is_vec_v<T>)
constexpr lak::vec4<T> operator*(T lhs, lak::vec4<T> rhs)
{
	rhs.x *= lhs;
	rhs.y *= lhs;
	rhs.z *= lhs;
	rhs.w *= lhs;
	return rhs;
}

template<typename T>
constexpr lak::vec2<T> &operator*=(lak::vec2<T> &lhs, const lak::vec2<T> &rhs)
{
	lhs.x *= rhs.x;
	lhs.y *= rhs.y;
	return lhs;
}

template<typename T>
requires(!lak::is_vec_v<T>)
constexpr lak::vec2<T> &operator*=(lak::vec2<T> &lhs, T rhs)
{
	lhs.x *= rhs;
	lhs.y *= rhs;
	return lhs;
}

template<typename T>
constexpr lak::vec3<T> &operator*=(lak::vec3<T> &lhs, const lak::vec3<T> &rhs)
{
	lhs.x *= rhs.x;
	lhs.y *= rhs.y;
	lhs.z *= rhs.z;
	return lhs;
}

template<typename T>
requires(!lak::is_vec_v<T>)
constexpr lak::vec3<T> &operator*=(lak::vec3<T> &lhs, T rhs)
{
	lhs.x *= rhs;
	lhs.y *= rhs;
	lhs.z *= rhs;
	return lhs;
}

template<typename T>
constexpr lak::vec4<T> &operator*=(lak::vec4<T> &lhs, const lak::vec4<T> &rhs)
{
	lhs.x *= rhs.x;
	lhs.y *= rhs.y;
	lhs.z *= rhs.z;
	lhs.w *= rhs.w;
	return lhs;
}

template<typename T>
requires(!lak::is_vec_v<T>)
constexpr lak::vec4<T> &operator*=(lak::vec4<T> &lhs, T rhs)
{
	lhs.x *= rhs;
	lhs.y *= rhs;
	lhs.z *= rhs;
	lhs.w *= rhs;
	return lhs;
}

// division

template<typename T>
constexpr lak::vec2<T> operator/(lak::vec2<T> lhs, T rhs)
{
	lhs.x /= rhs;
	lhs.y /= rhs;
	return lhs;
}

template<typename T>
constexpr lak::vec2<T> operator/(T lhs, lak::vec2<T> rhs)
{
	rhs.x = lhs / rhs.x;
	rhs.y = lhs / rhs.y;
	return rhs;
}

template<typename T>
constexpr lak::vec2<T> operator/(lak::vec2<T> lhs, lak::vec2<T> rhs)
{
	lhs.x /= rhs.x;
	lhs.y /= rhs.y;
	return lhs;
}

template<typename T>
constexpr lak::vec3<T> operator/(lak::vec3<T> lhs, T rhs)
{
	lhs.x /= rhs;
	lhs.y /= rhs;
	lhs.z /= rhs;
	return lhs;
}

template<typename T>
constexpr lak::vec3<T> operator/(T lhs, lak::vec3<T> rhs)
{
	rhs.x = lhs / rhs.x;
	rhs.y = lhs / rhs.y;
	rhs.z = lhs / rhs.z;
	return rhs;
}

template<typename T>
constexpr lak::vec3<T> operator/(lak::vec3<T> lhs, lak::vec3<T> rhs)
{
	lhs.x /= rhs.x;
	lhs.y /= rhs.y;
	lhs.z /= rhs.z;
	return lhs;
}

template<typename T>
constexpr lak::vec4<T> operator/(lak::vec4<T> lhs, T rhs)
{
	lhs.x /= rhs;
	lhs.y /= rhs;
	lhs.z /= rhs;
	lhs.w /= rhs;
	return lhs;
}

template<typename T>
constexpr lak::vec4<T> operator/(T lhs, lak::vec4<T> rhs)
{
	rhs.x = lhs / rhs.x;
	rhs.y = lhs / rhs.y;
	rhs.z = lhs / rhs.z;
	rhs.w = lhs / rhs.w;
	return rhs;
}

template<typename T>
constexpr lak::vec4<T> operator/(lak::vec4<T> lhs, lak::vec4<T> rhs)
{
	lhs.x /= rhs.x;
	lhs.y /= rhs.y;
	lhs.z /= rhs.z;
	lhs.w /= rhs.w;
	return lhs;
}

template<typename T>
constexpr lak::vec2<T> &operator/=(lak::vec2<T> &lhs, T rhs)
{
	lhs.x /= rhs;
	lhs.y /= rhs;
	return lhs;
}

template<typename T>
constexpr lak::vec2<T> &operator/=(lak::vec2<T> &lhs, lak::vec2<T> rhs)
{
	lhs.x /= rhs.x;
	lhs.y /= rhs.y;
	return lhs;
}

template<typename T>
constexpr lak::vec3<T> &operator/=(lak::vec3<T> &lhs, T rhs)
{
	lhs.x /= rhs;
	lhs.y /= rhs;
	lhs.z /= rhs;
	return lhs;
}

template<typename T>
constexpr lak::vec3<T> &operator/=(lak::vec3<T> &lhs, lak::vec3<T> rhs)
{
	lhs.x /= rhs.x;
	lhs.y /= rhs.y;
	lhs.z /= rhs.z;
	return lhs;
}

template<typename T>
constexpr lak::vec4<T> &operator/=(lak::vec4<T> &lhs, T rhs)
{
	lhs.x /= rhs;
	lhs.y /= rhs;
	lhs.z /= rhs;
	lhs.w /= rhs;
	return lhs;
}

template<typename T>
constexpr lak::vec4<T> &operator/=(lak::vec4<T> &lhs, lak::vec4<T> rhs)
{
	lhs.x /= rhs.x;
	lhs.y /= rhs.y;
	lhs.z /= rhs.z;
	lhs.w /= rhs.w;
	return lhs;
}

// addition

template<typename T>
constexpr lak::vec2<T> operator+(lak::vec2<T> lhs, const lak::vec2<T> &rhs)
{
	lhs.x += rhs.x;
	lhs.y += rhs.y;
	return lhs;
}

template<typename T>
constexpr lak::vec3<T> operator+(lak::vec3<T> lhs, const lak::vec3<T> &rhs)
{
	lhs.x += rhs.x;
	lhs.y += rhs.y;
	lhs.z += rhs.z;
	return lhs;
}

template<typename T>
constexpr lak::vec4<T> operator+(lak::vec4<T> lhs, const lak::vec4<T> &rhs)
{
	lhs.x += rhs.x;
	lhs.y += rhs.y;
	lhs.z += rhs.z;
	lhs.w += rhs.w;
	return lhs;
}

template<typename T>
constexpr lak::vec2<T> &operator+=(lak::vec2<T> &lhs, const lak::vec2<T> &rhs)
{
	lhs.x += rhs.x;
	lhs.y += rhs.y;
	return lhs;
}

template<typename T>
constexpr lak::vec3<T> &operator+=(lak::vec3<T> &lhs, const lak::vec3<T> &rhs)
{
	lhs.x += rhs.x;
	lhs.y += rhs.y;
	lhs.z += rhs.z;
	return lhs;
}

template<typename T>
constexpr lak::vec4<T> &operator+=(lak::vec4<T> &lhs, const lak::vec4<T> &rhs)
{
	lhs.x += rhs.x;
	lhs.y += rhs.y;
	lhs.z += rhs.z;
	lhs.w += rhs.w;
	return lhs;
}

// subtraction

template<typename T>
constexpr lak::vec2<T> operator-(lak::vec2<T> vec)
{
	vec.x = -vec.x;
	vec.y = -vec.y;
	return vec;
}

template<typename T>
constexpr lak::vec3<T> operator-(lak::vec3<T> vec)
{
	vec.x = -vec.x;
	vec.y = -vec.y;
	vec.z = -vec.z;
	return vec;
}

template<typename T>
constexpr lak::vec4<T> operator-(lak::vec4<T> vec)
{
	vec.x = -vec.x;
	vec.y = -vec.y;
	vec.z = -vec.z;
	vec.w = -vec.w;
	return vec;
}

template<typename T>
constexpr lak::vec2<T> operator-(lak::vec2<T> lhs, const lak::vec2<T> &rhs)
{
	lhs.x -= rhs.x;
	lhs.y -= rhs.y;
	return lhs;
}

template<typename T>
constexpr lak::vec3<T> operator-(lak::vec3<T> lhs, const lak::vec3<T> &rhs)
{
	lhs.x -= rhs.x;
	lhs.y -= rhs.y;
	lhs.z -= rhs.z;
	return lhs;
}

template<typename T>
constexpr lak::vec4<T> operator-(lak::vec4<T> lhs, const lak::vec4<T> &rhs)
{
	lhs.x -= rhs.x;
	lhs.y -= rhs.y;
	lhs.z -= rhs.z;
	lhs.w -= rhs.w;
	return lhs;
}

template<typename T>
constexpr lak::vec2<T> &operator-=(lak::vec2<T> &lhs, const lak::vec2<T> &rhs)
{
	lhs.x -= rhs.x;
	lhs.y -= rhs.y;
	return lhs;
}

template<typename T>
constexpr lak::vec3<T> &operator-=(lak::vec3<T> &lhs, const lak::vec3<T> &rhs)
{
	lhs.x -= rhs.x;
	lhs.y -= rhs.y;
	lhs.z -= rhs.z;
	return lhs;
}

template<typename T>
constexpr lak::vec4<T> &operator-=(lak::vec4<T> &lhs, const lak::vec4<T> &rhs)
{
	lhs.x -= rhs.x;
	lhs.y -= rhs.y;
	lhs.z -= rhs.z;
	lhs.w -= rhs.w;
	return lhs;
}

namespace lak
{
	// matrix transpose

	template<typename T, size_t R, size_t C>
	constexpr lak::mat<T, C, R> transpose(const lak::mat<T, R, C> &mat)
	{
		lak::mat<T, C, R> result;
		for (size_t c = 0U; c < C; ++c)
			for (size_t r = 0U; r < R; ++r) result[c][r] = mat[r][c];
		return result;
	}
}

// matrix multiplication

template<typename T, size_t C>
constexpr lak::vec<T, 2U> operator*(const lak::mat<T, 2U, C> &mat,
                                    const lak::vec<T, C> &vec)
{
	return {lak::dot(mat.x, vec), lak::dot(mat.y, vec)};
}

template<typename T, size_t C>
constexpr lak::vec<T, 3U> operator*(const lak::mat<T, 3U, C> &mat,
                                    const lak::vec<T, C> &vec)
{
	return {lak::dot(mat.x, vec), lak::dot(mat.y, vec), lak::dot(mat.z, vec)};
}

template<typename T, size_t C>
constexpr lak::vec<T, 4U> operator*(const lak::mat<T, 4U, C> &mat,
                                    const lak::vec<T, C> &vec)
{
	return {lak::dot(mat.x, vec),
	        lak::dot(mat.y, vec),
	        lak::dot(mat.z, vec),
	        lak::dot(mat.w, vec)};
}

template<typename T, size_t R, size_t C>
constexpr lak::vec<T, C> operator*(const lak::vec<T, R> &vec,
                                   const lak::mat<T, R, C> &mat)
{
	return lak::transpose(mat) * vec;
}

namespace lak
{
	// homogeneous matrix multiplication

	template<typename T>
	constexpr lak::vec2<T> homogeneous_mult(const lak::mat3<T> &mat,
	                                        const lak::vec2<T> &vec)
	{
		return lak::dehomogenise(mat * lak::homogenise(vec));
	}

	template<typename T>
	constexpr lak::vec3<T> homogeneous_mult(const lak::mat4<T> &mat,
	                                        const lak::vec3<T> &vec)
	{
		return lak::dehomogenise(mat * lak::homogenise(vec));
	}

	// matrix determinant

	template<typename T>
	constexpr T det(const lak::mat2<T> &mat)
	{
		return (mat.x.x * mat.y.y) - (mat.x.y * mat.y.x);
	}

	template<typename T>
	constexpr lak::vec3<T> det_part(const lak::mat<T, 2U, 3U> &mat)
	{
		const T _det_x = lak::det(lak::mat2<T>{mat.x.yz(), mat.y.yz()});
		const T _det_y = -lak::det(lak::mat2<T>{mat.x.xz(), mat.y.xz()});
		const T _det_z = lak::det(lak::mat2<T>{mat.x.xy(), mat.y.xy()});

		return lak::vec3<T>(_det_x, _det_y, _det_z);
	}

	template<typename T>
	constexpr T det(const lak::mat3<T> &mat)
	{
		return lak::dot(mat.x, lak::det_part(mat.yz()));
	}

	template<typename T>
	constexpr lak::vec4<T> det_part(const lak::mat<T, 3U, 4U> &mat)
	{
		const T _det_x =
		  lak::det(lak::mat3<T>(mat.x.yzw(), mat.y.yzw(), mat.z.yzw()));
		const T _det_y =
		  -lak::det(lak::mat3<T>(mat.x.xzw(), mat.y.xzw(), mat.z.xzw()));
		const T _det_z =
		  lak::det(lak::mat3<T>(mat.x.xyw(), mat.y.xyw(), mat.z.xyw()));
		const T _det_w =
		  -lak::det(lak::mat3<T>(mat.x.xyz(), mat.y.xyz(), mat.z.xyz()));

		return lak::vec4<T>(_det_x, _det_y, _det_z, _det_w);
	}

	template<typename T>
	constexpr T det(const lak::mat4<T> &mat)
	{
		return lak::dot(mat.x, lak::det_part(mat.yzw()));
	}

	// matrix inverse

	template<typename T>
	constexpr lak::mat2<T> inverse(const lak::mat2<T> &mat)
	{
		const T _det{lak::det(mat)};
		return {
		  lak::vec2<T>(mat.y.y / _det, -mat.x.y / _det),
		  lak::vec2<T>(-mat.y.x / _det, mat.x.x / _det),
		};
	}

	template<typename T>
	constexpr lak::mat3<T> inverse(const lak::mat3<T> &mat)
	{
		const lak::mat3<T> _det_mat{
		  lak::det_part(mat.yz()),
		  -lak::det_part(mat.xz()),
		  lak::det_part(mat.xy()),
		};

		const T _det = lak::dot(mat.x, _det_mat.x);

		const auto _det_mat_tr = lak::transpose(_det_mat);

		return {
		  _det_mat_tr.x / _det,
		  _det_mat_tr.y / _det,
		  _det_mat_tr.z / _det,
		};
	}

	template<typename T>
	constexpr lak::mat4<T> inverse(const lak::mat4<T> &mat)
	{
		const lak::mat4<T> _det_mat{
		  lak::det_part(mat.yzw()),
		  -lak::det_part(mat.xzw()),
		  lak::det_part(mat.xyw()),
		  -lak::det_part(mat.xyz()),
		};

		const T _det = lak::dot(mat.x, _det_mat.x);

		const auto _det_mat_tr = lak::transpose(_det_mat);

		return {
		  _det_mat_tr.x / _det,
		  _det_mat_tr.y / _det,
		  _det_mat_tr.z / _det,
		  _det_mat_tr.w / _det,
		};
	}
}

// comparisons

template<typename T>
constexpr bool operator==(const lak::vec2<T> &lhs, const lak::vec2<T> &rhs)
{
	return (lhs.x == rhs.x) && (lhs.y == rhs.y);
}

template<typename T>
constexpr bool operator==(const lak::vec3<T> &lhs, const lak::vec3<T> &rhs)
{
	return (lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z);
}

template<typename T>
constexpr bool operator==(const lak::vec4<T> &lhs, const lak::vec4<T> &rhs)
{
	return (lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z) &&
	       (lhs.w == rhs.w);
}

template<typename T>
constexpr bool operator!=(const lak::vec2<T> &lhs, const lak::vec2<T> &rhs)
{
	return !(lhs == rhs);
}

template<typename T>
constexpr bool operator!=(const lak::vec3<T> &lhs, const lak::vec3<T> &rhs)
{
	return !(lhs == rhs);
}

template<typename T>
constexpr bool operator!=(const lak::vec4<T> &lhs, const lak::vec4<T> &rhs)
{
	return !(lhs == rhs);
}

template<typename T, typename CHAR>
struct lak::format_traits<lak::vec2<T>, CHAR>
{
	static_assert(lak::concepts::formattable<T, CHAR>);

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
	static_assert(lak::concepts::formattable<T, CHAR>);

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
	static_assert(lak::concepts::formattable<T, CHAR>);

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
