#ifndef LAK_QUAT_HPP
#define LAK_QUAT_HPP

#ifdef __has_include
#	if __has_include(<glm/vec4.hpp>) && __has_include(<glm/mat4x4.hpp>)
#		include <glm/vec4.hpp>
#		include <glm/mat4x4.hpp>
#		define LAK_QUAT_HPP_HAS_GLM
#	endif
#endif

#include "lak/vec.hpp"

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <tuple>

namespace lak
{
	template<typename T>
	struct quat;
	template<typename T>
	struct dquat;

	template<typename T>
	struct quat
	{
		using value_type = T;

		value_type x = value_type(0);
		value_type y = value_type(0);
		value_type z = value_type(0);
		value_type w = value_type(0);

		quat()                        = default;
		quat(const quat &)            = default;
		quat &operator=(const quat &) = default;

		quat(const value_type (&values)[4])
		: x(values[0]), y(values[1]), z(values[2]), w(values[3])
		{
		}

		quat(const value_type X,
		     const value_type Y,
		     const value_type Z,
		     const value_type W)
		: x(X), y(Y), z(Z), w(W)
		{
		}

		value_type &operator[](const size_t i) { return (&x)[i]; }
		const value_type &operator[](const size_t i) const { return (&x)[i]; }

		template<size_t I>
		requires((I < 4))
		T &get()
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
		const T &get() const
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

		template<typename U>
		explicit operator lak::quat<U>() const
		{
			return lak::quat<U>{static_cast<U>(x),
			                    static_cast<U>(y),
			                    static_cast<U>(z),
			                    static_cast<U>(w)};
		}

		template<typename U>
		explicit operator lak::vec4<U>() const
		{
			return lak::vec4<U>{static_cast<U>(x),
			                    static_cast<U>(y),
			                    static_cast<U>(z),
			                    static_cast<U>(w)};
		}

#ifdef LAK_QUAT_HPP_HAS_GLM
		template<typename GLMT, glm::precision GLMP>
		explicit quat(const glm::tvec4<GLMT, GLMP> &v)
		{
			x = static_cast<value_type>(v.x);
			y = static_cast<value_type>(v.y);
			z = static_cast<value_type>(v.z);
			w = static_cast<value_type>(v.w);
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

		/* --- euler vector --- */

		lak::vec4<value_type> euler_vector() const
		{
			const value_type htheta =
			  std::atan2(std::sqrt((x * x) + (y * y) + (z * z)), w);
			const value_type denom = std::sin(htheta);
			lak::vec4<value_type> result{
			  value_type(0), value_type(0), value_type(0), value_type(2) * htheta};
			if (denom != value_type(0)) [[likely]]
			{
				result.x = x / denom;
				result.y = y / denom;
				result.z = z / denom;
			}
			return result;
		}

		static quat from_euler_vector(const lak::vec4<value_type> &v)
		{
			const value_type w2  = v.w / value_type(2);
			const value_type sw2 = std::sin(w2);
			return {v.x * sw2, v.y * sw2, v.z * sw2, std::cos(w2)};
		}

		/* --- euler angles --- */

		lak::vec3<value_type> euler_angles() const
		{
			const value_type sy = value_type(2) * ((w * y) - (z * x));
			return {
			  std::atan2(value_type(2) * ((w * x) + (y * z)),
			             value_type(1) - (value_type(2) * ((x * x) + (y * y)))),
			  std::fabs(sy) >= value_type(1) ? std::copysign(value_type(M_PI_2), sy)
			                                 : std::asin(sy),
			  std::atan2(value_type(2) * ((w * z) + (x * y)),
			             value_type(1) - (value_type(2) * ((y * y) + (z * z))))};
		}

		static quat from_euler_angles(const lak::vec3<value_type> &v)
		{
			const auto v2       = v * value_type(0.5);
			const value_type cx = std::cos(v2.x), sx = std::sin(v2.x),
			                 cy = std::cos(v2.y), sy = std::sin(v2.y),
			                 cz = std::cos(v2.z), sz = std::sin(v2.z);
			return {(cx * sy * cz) - (sx * cy * sz),
			        (cx * cy * sz) + (sx * sy * cz),
			        (sx * cy * cz) - (cx * sy * sz),
			        (cx * cy * cz) + (sx * sy * sz)};
		}

		quat &rotate(const lak::vec3<value_type> &v)
		{
			return *this *= from_euler_angles(v);
		}
		quat rotated(const lak::vec3<value_type> &v)
		{
			return *this * from_euler_angles(v);
		}

		/* --- conjugate --- */

		quat conj() const { return {-x, -y, -z, w}; }
		quat operator*() const { return conj(); }

		/* --- magnitude --- */

		value_type mag() const
		{
			return std::sqrt((x * x) + (y * y) + (z * z) + (w * w));
		}
		value_type operator~() const { return mag(); }

		/* --- normalise --- */

		quat &normalise() { return *this *= (value_type(1) / mag()); }
		quat norm() const { return *this *= (value_type(1) / mag()); }
		quat operator!() const { return norm(); }

		/* --- addition --- */

		quat operator+(const quat &rhs) const
		{
			return {x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w};
		}
		quat &operator+=(const quat &rhs)
		{
			x += rhs.x;
			y += rhs.y;
			z += rhs.z;
			w += rhs.w;
			return *this;
		}

		/* --- substraction --- */

		quat operator-(const quat &rhs) const
		{
			return {x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w};
		}
		quat &operator-=(const quat &rhs)
		{
			x -= rhs.x;
			y -= rhs.y;
			z -= rhs.z;
			w -= rhs.w;
			return *this;
		}

		/* --- scalar multiplication --- */

		quat operator*(const value_type rhs) const
		{
			return {x * rhs, y * rhs, z * rhs, w * rhs};
		}
		quat &operator*=(const value_type rhs) const
		{
			x *= rhs;
			y *= rhs;
			z *= rhs;
			w *= rhs;
			return *this;
		}
		friend quat operator*(const value_type lhs, const quat &rhs)
		{
			return {lhs * rhs.x, lhs * rhs.y, lhs * rhs.z, lhs * rhs.w};
		}

		/* --- quaternion multiplication --- */

		quat operator*(const quat &rhs) const
		{
			const value_type _w =
			  (w * rhs.w) - (x * rhs.x) - (y * rhs.y) - (z * rhs.z);
			const value_type _x =
			  (w * rhs.x) + (x * rhs.w) + (y * rhs.z) - (z * rhs.y);
			const value_type _y =
			  (w * rhs.y) - (x * rhs.z) + (y * rhs.w) + (z * rhs.x);
			const value_type _z =
			  (w * rhs.z) + (x * rhs.y) - (y * rhs.x) + (z * rhs.w);
			return {_x, _y, _z, _w};
		}
		quat &operator*=(const quat &rhs)
		{
			const value_type _w =
			  (w * rhs.w) - (x * rhs.x) - (y * rhs.y) - (z * rhs.z);
			const value_type _x =
			  (w * rhs.x) + (x * rhs.w) + (y * rhs.z) - (z * rhs.y);
			const value_type _y =
			  (w * rhs.y) - (x * rhs.z) + (y * rhs.w) + (z * rhs.x);
			const value_type _z =
			  (w * rhs.z) + (x * rhs.y) - (y * rhs.x) + (z * rhs.w);
			x = _x;
			y = _y;
			z = _z;
			w = _w;
			return *this;
		}

		/* --- scalar division --- */

		quat operator/(const value_type rhs) const
		{
			return {x / rhs, y / rhs, z / rhs, w / rhs};
		}
		quat &operator/=(const value_type rhs)
		{
			x /= rhs;
			y /= rhs;
			z /= rhs;
			w /= rhs;
			return *this;
		}
		friend quat operator/(const value_type lhs, const quat &rhs)
		{
			return lhs * *rhs;
		}

		/* --- quaternion division --- */

		quat operator/(const quat &rhs) const { return *this * !*rhs; }
		quat operator/=(const quat &rhs) { return *this *= !*rhs; }
	};

	template<typename T>
	struct dquat
	{
		using value_type = T;
		using quat_type  = lak::quat<value_type>;

		quat_type real;
		quat_type dual;

		dquat() : real(0, 0, 0, 1), dual(0, 0, 0, 0) {}
		dquat(const dquat &)            = default;
		dquat &operator=(const dquat &) = default;

		dquat(const quat_type (&values)[2]) : real(values[0]), dual(values[1]) {}

		dquat(const value_type (&values)[8])
		: real(values[0], values[1], values[2], values[3]),
		  dual(values[4], values[5], values[6], values[7])
		{
		}

		dquat(const quat_type &r, const quat_type &d) : real(r), dual(d) {}

		dquat(const value_type rx,
		      const value_type ry,
		      const value_type rz,
		      const value_type rw,
		      const value_type dx,
		      const value_type dy,
		      const value_type dz,
		      const value_type dw)
		: real(rx, ry, rz, rw), dual(dx, dy, dz, dw)
		{
		}

		template<size_t I>
		requires((I < 2))
		T &get()
		{
			if constexpr (I == 0)
				return real;
			else if constexpr (I == 1)
				return dual;
		}

		template<size_t I>
		requires((I < 2))
		const T &get() const
		{
			if constexpr (I == 0)
				return real;
			else if constexpr (I == 1)
				return dual;
		}

		template<typename U>
		explicit operator lak::dquat<U>() const
		{
			return lak::dquat<U>{static_cast<lak::quat<U>>(real),
			                     static_cast<lak::quat<U>>(dual)};
		}

		/* --- transform --- */

		quat_type transform(const quat_type &q) const
		{
			return (real * q * *real) + translation();
		}

		lak::mat4<value_type> transform_matrix() const
		{
			// this is a Euler-Rodrigues transform
			// https://en.wikipedia.org/wiki/Euler-Rodrigues_formula

			const auto tq = translation();

			const value_type x2  = real.x * real.x;
			const value_type y2  = real.y * real.y;
			const value_type z2  = real.z * real.z;
			const value_type w2  = real.w * real.w;
			const value_type tx  = value_type(2) * real.x;
			const value_type ty  = value_type(2) * real.y;
			const value_type tz  = value_type(2) * real.z;
			const value_type txy = tx * real.y;
			const value_type txz = tx * real.z;
			const value_type txw = tx * real.w;
			const value_type tyz = ty * real.z;
			const value_type tyw = ty * real.w;
			const value_type tzw = tz * real.w;

			return {
			  {
			    (x2 + w2) - (y2 + z2),
			    txy - tzw,
			    txz + tyw,
			    tq.x,
			  },
			  {
			    txy + tzw,
			    (y2 + w2) - (x2 + z2),
			    tyz - txw,
			    tq.y,
			  },
			  {
			    txz - tyw,
			    tyz + txw,
			    (z2 + w2) - (x2 + y2),
			    tq.z,
			  },
			  {
			    value_type(0),
			    value_type(0),
			    value_type(0),
			    value_type(1),
			  },
			};
		}

		explicit operator lak::mat4<value_type>() const
		{
			return transform_matrix();
		}

#ifdef LAK_QUAT_HPP_HAS_GLM
		template<typename GLMT, glm::precision GLMP>
		explicit operator glm::tmat4x4<GLMT, GLMP>() const
		{
			const auto m = transform_matrix();
			return {
			  static_cast<GLMT>(m.x.x),
			  static_cast<GLMT>(m.y.x),
			  static_cast<GLMT>(m.z.x),
			  static_cast<GLMT>(m.w.x),

			  static_cast<GLMT>(m.x.y),
			  static_cast<GLMT>(m.y.y),
			  static_cast<GLMT>(m.z.y),
			  static_cast<GLMT>(m.w.y),

			  static_cast<GLMT>(m.x.z),
			  static_cast<GLMT>(m.y.z),
			  static_cast<GLMT>(m.z.z),
			  static_cast<GLMT>(m.w.z),

			  static_cast<GLMT>(m.x.w),
			  static_cast<GLMT>(m.y.w),
			  static_cast<GLMT>(m.z.w),
			  static_cast<GLMT>(m.w.w),
			};
		}
#endif

		/* --- translation --- */

		quat_type translation() const
		{
			quat_type result = (dual * *real) * value_type(2);
			result.w         = value_type(0);
			return result;
		}

		static dquat from_translation(const quat_type &q)
		{
			const quat_type trans{q.x, q.y, q.z, value_type(0)};
			const quat_type real{0, 0, 0, 1};
			return {real, (trans * real) * value_type(0.5)};
		}

		lak::vec3<value_type> translation_vec() const
		{
			quat_type q = (dual * *real) * value_type(2);
			return {q.x, q.y, q.z};
		}

		static dquat from_translation(const lak::vec3<value_type> &v)
		{
			const quat_type trans = {v.x, v.y, v.z, value_type(0)};
			const quat_type real{0, 0, 0, 1};
			return {real, (trans * real) * value_type(0.5)};
		}

		/* --- euler vector --- */

		lak::vec4<value_type> euler_vector() const { return real.euler_vector(); }

		static dquat from_euler_vector(const lak::vec4<value_type> &v)
		{
			return {quat_type::from_euler_vector(v), quat_type{0, 0, 0, 0}};
		}

		/* --- euler angles --- */

		lak::vec3<value_type> euler_angles() const { return real.euler_angles(); }

		static dquat from_euler_angles(const lak::vec3<value_type> &v)
		{
			return {quat_type::from_euler_angles(v), quat_type{0, 0, 0, 0}};
		}

		/* --- conjugate --- */

		dquat conj() const { return {*real, *dual}; }
		dquat operator*() const { return conj(); }

		/* --- magnitude --- */

		value_type mag() const { return real.mag(); }
		value_type operator~() const { return mag(); }

		/* --- normalise --- */

		dquat &normalise() { return *this *= (value_type(1) / mag()); }
		dquat norm() const { return *this * (value_type(1) / mag()); }
		dquat operator!() const { return norm(); }

		/* --- addition --- */

		dquat operator+(const dquat &rhs) const
		{
			return {real + rhs.real, dual + rhs.dual};
		}
		dquat &operator+=(const dquat &rhs)
		{
			real += rhs.real;
			dual += rhs.dual;
			return *this;
		}

		/* --- subtraction --- */

		dquat operator-(const dquat &rhs) const
		{
			return {real - rhs.real, dual - rhs.dual};
		}
		dquat &operator-=(const dquat &rhs)
		{
			real -= rhs.real;
			dual -= rhs.dual;
			return *this;
		}

		/* --- scalar multiplication --- */

		dquat operator*(const value_type rhs) const
		{
			return {real * rhs, dual * rhs};
		}
		dquat &operator*=(const value_type rhs)
		{
			real *= rhs;
			dual *= rhs;
			return *this;
		}
		friend dquat operator*(const value_type lhs, const dquat &rhs)
		{
			return {lhs * rhs.real, lhs * rhs.dual};
		}

		/* --- quaternion multiplication --- */

		dquat operator*(const dquat &rhs) const
		{
			const quat_type d = rhs.transform(translation()) * value_type(0.5);
			const quat_type r = real * rhs.real;
			return {r, d * r};
		}
		dquat &operator*=(const dquat &rhs)
		{
			const quat_type d = rhs.transform(translation()) * value_type(0.5);
			real *= rhs.real;
			dual = d * real;
			return *this;
		}

		/* --- scalar division --- */

		dquat operator/(const value_type rhs) const
		{
			return {real / rhs, dual / rhs};
		}
		dquat operator/=(const value_type rhs)
		{
			real /= rhs;
			dual /= rhs;
			return *this;
		}

		/* --- quaternion division --- */

		dquat operator/(const dquat &rhs) const { return *this * !*rhs; }
		dquat &operator/=(const dquat &rhs) const { return *this *= !*rhs; }
	};

	using quatf_t  = quat<float>;
	using quatd_t  = quat<double>;
	using quat32_t = quat<f32_t>;
	using quat64_t = quat<f64_t>;

	using dquatf_t  = dquat<float>;
	using dquatd_t  = dquat<double>;
	using dquat32_t = dquat<f32_t>;
	using dquat64_t = dquat<f64_t>;
}

namespace std
{
	template<typename T>
	struct tuple_size<lak::quat<T>>
	{
		static constexpr size_t value = 4;
	};
	template<size_t I, typename T>
	struct tuple_element<I, lak::quat<T>>
	{
		using type = lak::quat<T>::value_type;
	};

	template<typename T>
	struct tuple_size<lak::dquat<T>>
	{
		static constexpr size_t value = 2;
	};
	template<size_t I, typename T>
	struct tuple_element<I, lak::dquat<T>>
	{
		using type = lak::dquat<T>::quat_type;
	};
}

template<typename T, typename CHAR>
struct lak::format_traits<lak::quat<T>, CHAR>
{
	static_assert(lak::concepts::formattable<T, CHAR>);

	using format_args = lak::format_args_t<T, CHAR>;

	static consteval format_args parse_args(lak::string_view<CHAR> fmt)
	{
		return lak::parse_format_args<T>(fmt);
	}

	static constexpr lak::string<CHAR> to_string(const format_args &args,
	                                             const lak::quat<T> &val)
	requires(lak::concepts::dynamic_formattable<T, CHAR>)
	{
		return lak::fmt<CHAR, "({} {} {} {})">(
		  lak::format_traits<T, CHAR>::to_string(args, val.x),
		  lak::format_traits<T, CHAR>::to_string(args, val.y),
		  lak::format_traits<T, CHAR>::to_string(args, val.z),
		  lak::format_traits<T, CHAR>::to_string(args, val.w));
	}

	static constexpr lak::string<CHAR> to_string(const lak::quat<T> &val)
	requires(!lak::concepts::dynamic_formattable<T, CHAR>)
	{
		return lak::fmt<CHAR, "({} {} {} {})">(val.x, val.y, val.z, val.w);
	}
};

template<typename T, typename CHAR>
struct lak::format_traits<lak::dquat<T>, CHAR>
{
	static_assert(lak::concepts::formattable<T, CHAR>);

	using format_args = lak::format_args_t<T, CHAR>;

	static consteval format_args parse_args(lak::string_view<CHAR> fmt)
	{
		return lak::parse_format_args<T>(fmt);
	}

	static constexpr lak::string<CHAR> to_string(const format_args &args,
	                                             const lak::dquat<T> &val)
	requires(lak::concepts::dynamic_formattable<T, CHAR>)
	{
		return lak::fmt<CHAR, "({} {})">(
		  lak::format_traits<T, CHAR>::to_string(args, val.real),
		  lak::format_traits<T, CHAR>::to_string(args, val.dual));
	}

	static constexpr lak::string<CHAR> to_string(const lak::dquat<T> &val)
	requires(!lak::concepts::dynamic_formattable<T, CHAR>)
	{
		return lak::fmt<CHAR, "({} {})">(val.real, val.dual);
	}
};

LAK_FIXED_TEMPLATE_STRUCT_BYTES_TRAITS(typename T,
                                       lak::quat<T>,
                                       &lak::quat<T>::x,
                                       &lak::quat<T>::y,
                                       &lak::quat<T>::z,
                                       &lak::quat<T>::w);

LAK_FIXED_TEMPLATE_STRUCT_BYTES_TRAITS(typename T,
                                       lak::dquat<T>,
                                       &lak::dquat<T>::real,
                                       &lak::dquat<T>::dual);

#endif
