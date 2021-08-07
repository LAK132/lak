#ifndef LAK_BUFFER_HPP
#define LAK_BUFFER_HPP

#include "lak/stdint.hpp"

namespace lak
{
	template<typename T, size_t S>
	struct buffer
	{
		T _buffer[S] = {};
		size_t _pos  = 0;
		size_t _size = 0;

		buffer()               = default;
		buffer(const buffer &) = default;
		~buffer()              = default;
		buffer &operator=(const buffer &) = default;

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
	};
}

#endif