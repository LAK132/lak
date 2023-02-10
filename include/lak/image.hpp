#ifndef LAK_IMAGE_HPP
#define LAK_IMAGE_HPP

#include "lak/array.hpp"
#include "lak/colour.hpp"
#include "lak/vec.hpp"

#include <cstring>
#include <vector>

namespace lak
{
	template<typename T>
	struct image;
	template<typename T>
	struct image_view;
	template<typename T>
	struct image_subview;

	template<typename T>
	struct image
	{
		using size_type  = vec2s_t;
		using value_type = T;

		image() = default;
		image(size_type size) { resize(size); }

		image(const image &)            = default;
		image &operator=(const image &) = default;

		image(image &&img) : _size(img._size), _value(lak::move(img._value)) {}
		image &operator=(image &&img)
		{
			lak::swap(_size, img._size);
			lak::swap(_value, img._value);
			return *this;
		}

		value_type &at(const size_type index)
		{
			return _value[index.x + (_size.x * index.y)];
		}

		const value_type &at(const size_type index) const
		{
			return _value[index.x + (_size.x * index.y)];
		}

		value_type &at(const size_t index) { return _value[index]; }

		const value_type &at(const size_t index) const { return _value[index]; }

		value_type &operator[](const size_type index) { return at(index); }

		const value_type &operator[](const size_type index) const
		{
			return at(index);
		}

		value_type &operator[](const size_t index) { return at(index); }

		const value_type &operator[](const size_t index) const
		{
			return at(index);
		}

		void resize(const size_type sz)
		{
			_size = sz;
			_value.resize(sz.x * sz.y);
		}

		size_type size() const { return _size; }

		size_t contig_size() const { return _value.size(); }

		size_t contig_size_bytes() const { return contig_size() * sizeof(T); }

		value_type *data() { return _value.data(); }

		const value_type *data() const { return _value.data(); }

		void fill(const value_type &value)
		{
			for (auto &v : _value) v = value;
		}

		template<lak::endian E = lak::endian::little>
		static lak::result<image> from_bytes(lak::span<const byte_t> bytes,
		                                     size_type size)
		{
			return lak::array_from_bytes<value_type, E>(bytes, size.x * size.y)
			  .map(
			    [&size](lak::array<value_type> &&data) -> image
			    {
				    image result;
				    result._value = lak::move(data);
				    result._size  = size;
				    ASSERT_EQUAL(result._value.size(),
				                 result._size.x * result._size.y);
				    return result;
			    });
		}

	private:
		size_type _size;
		lak::array<value_type> _value;
	};

	template<typename T>
	struct image_view
	{
		using size_type  = lak::vec2s_t;
		using value_type = T;

		image_view() = default;

		template<typename U,
		         lak::enable_if_i<lak::is_same_v<lak::remove_const_t<T>,
		                                         lak::remove_const_t<U>> &&
		                          (lak::is_const_v<T> || !lak::is_const_v<U>)> = 0>
		image_view(const image_view<U> &img)
		: _value(img._value), _size(img._size), _padding(img._padding)
		{
		}

		template<typename U,
		         lak::enable_if_i<lak::is_same_v<lak::remove_const_t<T>,
		                                         lak::remove_const_t<U>> &&
		                          (lak::is_const_v<T> || !lak::is_const_v<U>)> = 0>
		image_view &operator=(const image_view<U> &img)
		{
			_value   = img._value;
			_size    = img._size;
			_padding = img._padding;
			return *this;
		}

		template<typename U, lak::enable_if_i<lak::is_same_v<T, const U>> = 0>
		image_view(const image<U> &img)
		: _value(lak::span(img.data(), img.contig_size())), _size(img.size())
		{
			ASSERT_GREATER_OR_EQUAL(_value.size_bytes(),
			                        ((sizeof(value_type) * _size.x) + _padding) *
			                          _size.y);
		}

		template<typename U, lak::enable_if_i<lak::is_same_v<T, U>> = 0>
		image_view(image<U> &img)
		: _value(lak::span(img.data(), img.contig_size())), _size(img.size())
		{
			ASSERT_GREATER_OR_EQUAL(_value.size_bytes(),
			                        ((sizeof(value_type) * _size.x) + _padding) *
			                          _size.y);
		}

		image_view(lak::span<T> pixels,
		           size_type size,
		           uintptr_t row_end_padding = 0)
		: _value(pixels), _size(size), _padding(row_end_padding)
		{
			ASSERT_GREATER_OR_EQUAL(_value.size_bytes(),
			                        ((sizeof(value_type) * _size.x) + _padding) *
			                          _size.y);
		}

		value_type &at(size_t index) const
		{
			auto base = reinterpret_cast<uintptr_t>(_value.data()) +
			            ((index / _size.x) * _padding);
			ASSERT_LESS(base + (sizeof(value_type) * index),
			            reinterpret_cast<uintptr_t>(_value.data()) +
			              _value.size_bytes());
			return reinterpret_cast<value_type *>(base)[index];
		}

		value_type &at(size_type index) const
		{
			size_t contig_index = index.x + (_size.x * index.y);
			auto base =
			  reinterpret_cast<uintptr_t>(_value.data()) + (index.y * _padding);
			ASSERT_LESS(base + (sizeof(value_type) * contig_index),
			            reinterpret_cast<uintptr_t>(_value.data()) +
			              _value.size_bytes());
			return reinterpret_cast<value_type *>(base)[contig_index];
		}

		value_type &operator[](size_t index) const { return at(index); }

		value_type &operator[](size_type index) const { return at(index); }

		size_type size() const { return _size; }

		uintptr_t padding() const { return _padding; }

		image_subview<T> subview(size_type from, size_type to) const
		{
			ASSERT_GREATER_OR_EQUAL(to.x, from.x);
			ASSERT_GREATER_OR_EQUAL(to.y, from.y);
			ASSERT_GREATER_OR_EQUAL(_size.x, to.x);
			ASSERT_GREATER_OR_EQUAL(_size.y, to.y);

			return {*this, from, to};
		}

		lak::span<lak::copy_const_t<T, void>> data() const { return _value; }

	protected:
		template<typename U>
		friend struct image_view;

		lak::span<lak::copy_const_t<T, void>> _value = {};
		size_type _size                              = {0, 0};
		uintptr_t _padding                           = 0;
	};

	template<typename T>
	image_view(const image<T> &) -> image_view<const T>;

	template<typename T>
	image_view(image<T> &) -> image_view<T>;

	template<typename T>
	image_view(lak::span<T>, lak::vec2s_t) -> image_view<T>;

	template<typename T>
	image_view(const image_view<T> &) -> image_view<T>;

	template<typename T>
	struct image_subview
	{
		using size_type  = lak::vec2s_t;
		using value_type = T;

		image_subview() = default;

		template<typename U,
		         lak::enable_if_i<lak::is_same_v<lak::remove_const_t<T>,
		                                         lak::remove_const_t<U>> &&
		                          (lak::is_const_v<T> || !lak::is_const_v<U>)> = 0>
		image_subview(const image_subview<U> &img)
		: _value(img._value), _start(img._start), _size(img._size)
		{
		}

		template<typename U,
		         lak::enable_if_i<lak::is_same_v<lak::remove_const_t<T>,
		                                         lak::remove_const_t<U>> &&
		                          (lak::is_const_v<T> || !lak::is_const_v<U>)> = 0>
		image_subview &operator=(const image_subview<U> &img)
		{
			_value = img._value;
			_start = img._start;
			_size  = img._size;
			return *this;
		}

		template<typename U, lak::enable_if_i<lak::is_same_v<T, const U>> = 0>
		image_subview(const image<U> &img)
		: _value(img), _start({0, 0}), _size(img.size())
		{
		}

		template<typename U, lak::enable_if_i<lak::is_same_v<T, U>> = 0>
		image_subview(image<U> &img)
		: _value(img), _start({0, 0}), _size(img.size())
		{
		}

		template<typename U,
		         lak::enable_if_i<lak::is_same_v<lak::remove_const_t<T>,
		                                         lak::remove_const_t<U>> &&
		                          (lak::is_const_v<T> || !lak::is_const_v<U>)> = 0>
		image_subview(const image_view<U> &img)
		: _value(img), _start({0, 0}), _size(img.size())
		{
		}

		template<typename U,
		         lak::enable_if_i<lak::is_same_v<lak::remove_const_t<T>,
		                                         lak::remove_const_t<U>> &&
		                          (lak::is_const_v<T> || !lak::is_const_v<U>)> = 0>
		image_subview(const image_view<U> &img, size_type min, size_type max)
		: _value(img), _start(min), _size(max.x - min.x, max.y - min.y)
		{
			ASSERT_GREATER_OR_EQUAL(_value.size().x, _start.x + _size.x);
			ASSERT_GREATER_OR_EQUAL(_value.size().y, _start.y + _size.y);
		}

		template<typename U, lak::enable_if_i<lak::is_same_v<T, const U>> = 0>
		image_subview(const image<U> &img, size_type min, size_type max)
		: _value(img), _start(min), _size(max.x - min.x, max.y - min.y)
		{
			ASSERT_GREATER_OR_EQUAL(_value.size().x, _start.x + _size.x);
			ASSERT_GREATER_OR_EQUAL(_value.size().y, _start.y + _size.y);
		}

		template<typename U, lak::enable_if_i<lak::is_same_v<T, U>> = 0>
		image_subview(const image<U> &img, size_type min, size_type max)
		: _value(img), _start(min), _size(max.x - min.x, max.y - min.y)
		{
			ASSERT_GREATER_OR_EQUAL(_value.size().x, _start.x + _size.x);
			ASSERT_GREATER_OR_EQUAL(_value.size().y, _start.y + _size.y);
		}

		template<typename U,
		         lak::enable_if_i<lak::is_same_v<lak::remove_const_t<T>,
		                                         lak::remove_const_t<U>> &&
		                          (lak::is_const_v<T> || !lak::is_const_v<U>)> = 0>
		image_subview(const image_subview<U> &img, size_type min, size_type max)
		: _value(img._value),
		  _start(img._start + min),
		  _size(max.x - min.x, max.y - min.y)
		{
			ASSERT_GREATER_OR_EQUAL(_value.size().x, _start.x + _size.x);
			ASSERT_GREATER_OR_EQUAL(_value.size().y, _start.y + _size.y);
		}

		value_type &at(size_t index) const
		{
			return at({index % _size.x, index / _size.x});
		}

		value_type &at(size_type index) const
		{
			return _value[{index.x + _start.x, index.y + _start.y}];
		}

		value_type &operator[](size_t index) const { return at(index); }

		value_type &operator[](size_type index) const { return at(index); }

		size_type size() const { return _size; }

		image_subview<T> subview(size_type from, size_type to) const
		{
			ASSERT_GREATER_OR_EQUAL(to.x, from.x);
			ASSERT_GREATER_OR_EQUAL(to.y, from.y);
			ASSERT_GREATER_OR_EQUAL(_size.x, to.x);
			ASSERT_GREATER_OR_EQUAL(_size.y, to.y);

			return {_value, from, to};
		}

	protected:
		template<typename U>
		friend struct image_subview;

		lak::image_view<T> _value = {};
		size_type _start          = {0, 0};
		size_type _size           = {0, 0};
	};

	template<typename T>
	image_subview(const image<T> &) -> image_subview<const T>;

	template<typename T>
	image_subview(image<T> &) -> image_subview<T>;

	template<typename T>
	image_subview(const image_view<T> &) -> image_subview<T>;

	template<typename T>
	image_subview(const image_subview<T> &) -> image_subview<T>;

	template<typename T, typename U>
	void blit(lak::image_subview<T> to, lak::image_subview<U> from)
	{
		static_assert(!lak::is_const_v<T>);
		// static_assert(lak::is_colour_v<T>);
		// static_assert(lak::is_colour_v<lak::remove_const_t<U>>);
		const size_t x_end = std::min(to.size().x, from.size().x);
		const size_t y_end = std::min(to.size().y, from.size().y);

		for (size_t y = 0; y < y_end; ++y)
		{
			for (size_t x = 0; x < x_end; ++x)
			{
				to[{x, y}] = T(from[{x, y}]);
			}
		}
	}

	template<typename T, typename U>
	void blit_blend(lak::image_subview<T> to, lak::image_subview<U> from)
	{
		static_assert(!lak::is_const_v<T>);
		static_assert(lak::is_colour_v<T>);
		static_assert(lak::is_colour_v<lak::remove_const_t<U>>);
		const size_t x_end = std::min(to.size().x, from.size().x);
		const size_t y_end = std::min(to.size().y, from.size().y);

		for (size_t y = 0; y < y_end; ++y)
		{
			for (size_t x = 0; x < x_end; ++x)
			{
				auto &c = to[{x, y}];
				c       = T(c % from[{x, y}]);
			}
		}
	}

	template<typename T, typename U>
	void blit_mul(lak::image_subview<T> to, lak::image_subview<U> from)
	{
		static_assert(!lak::is_const_v<T>);
		static_assert(lak::is_colour_v<T>);
		static_assert(lak::is_colour_v<lak::remove_const_t<U>>);
		const size_t x_end = std::min(to.size().x, from.size().x);
		const size_t y_end = std::min(to.size().y, from.size().y);

		for (size_t y = 0; y < y_end; ++y)
		{
			for (size_t x = 0; x < x_end; ++x)
			{
				auto &c = to[{x, y}];
				c       = T(c * from[{x, y}]);
			}
		}
	}

#define LAK_TEMPLATE_IMAGE_TYPEDEF(COLOUR, ...)                               \
	using COLOUR##image         = image<lak::colour::COLOUR>;                   \
	using COLOUR##image_view    = image_view<lak::colour::COLOUR>;              \
	using COLOUR##image_subview = image_subview<lak::colour::COLOUR>;
	LAK_FOREACH_COLOUR(LAK_TEMPLATE_IMAGE_TYPEDEF)

	using image3_t = image<color3_t>;
	using image4_t = image<color4_t>;

	[[maybe_unused]] inline image3_t image_from_rgb24(const byte_t *pixels,
	                                                  vec2s_t size)
	{
		return image3_t::from_bytes(
		         lak::span<const byte_t>(pixels, size.x * size.y * 3), size)
		  .unwrap();
	}

	[[maybe_unused]] inline image4_t image_from_rgba32(const byte_t *pixels,
	                                                   vec2s_t size)
	{
		return image4_t::from_bytes(
		         lak::span<const byte_t>(pixels, size.x * size.y * 4), size)
		  .unwrap();
	}
}

#include "lak/image.inl"

#endif
