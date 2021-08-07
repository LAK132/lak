#ifndef LAK_BUFFER_SPAN_HPP
#define LAK_BUFFER_SPAN_HPP

#include "lak/span.hpp"

namespace lak
{
	template<typename T, size_t S>
	struct buffer_span
	{
		lak::span<T, S> _buffer;
		size_t _pos  = 0;
		size_t _size = 0;

		buffer_span()                    = default;
		buffer_span(const buffer_span &) = default;
		~buffer_span()                   = default;
		buffer_span &operator=(const buffer_span &) = default;

		buffer_span(lak::span<T, S> buffer) : _buffer(buffer) {}

		const T *front() const { return _size > 0 ? _buffer + _pos : nullptr; }
		T *front() { return _size > 0 ? _buffer + _pos : nullptr; }

		const T *back() const
		{
			return _size > 0 ? _buffer + ((_pos + _size) % S) : nullptr;
		}
		T *back() { return _size > 0 ? _buffer + ((_pos + _size) % S) : nullptr; }

		bool pop_front()
		{
			if (_size == 0) return false;

			_pos = (_pos + 1) % S;
			--_size;

			return true;
		}
		bool pop_back()
		{
			if (_size == 0) return false;

			--_size;

			return true;
		}

		void push_back(const T &t)
		{
			_buffer[(_pos + _size) % S] = t;

			if (_size < S)
				++_size;
			else
				_pos = (_pos + 1) % S;
		}

		size_t size() const { return _size; }

		T &operator[](size_t index) { return _buffer[(_pos + index) % S]; }
		const T &operator[](size_t index) const
		{
			return _buffer[(_pos + index) % S];
		}
	};
}

#endif