#ifndef LAK_MEMORY_HPP
#define LAK_MEMORY_HPP

#include "lak/algorithm.hpp"
#include "lak/array.hpp"
#include "lak/compiler.hpp"
#include "lak/span.hpp"
#include "lak/stdint.hpp"
#include "lak/string.hpp"
#include "lak/type_traits.hpp"

#include <vector>

// wtf windows???
#ifdef min
#  undef min
#endif
#ifdef max
#  undef max
#endif

namespace lak
{
  template<typename T>
  T from_ne_bytes(lak::span<const uint8_t, sizeof(T)> bytes)
  {
    return *reinterpret_cast<const T *>(bytes.data());
  }

  template<typename T>
  T from_le_bytes(lak::span<const uint8_t, sizeof(T)> bytes)
  {
    T result = 0;
    for (size_t i = 0; i < bytes.size(); ++i) result |= bytes[i] << i;
    return result;
  }

  template<typename T>
  T from_be_bytes(lak::span<const uint8_t, sizeof(T)> bytes)
  {
    T result = 0;
    for (size_t i = 0; i++ < bytes.size();)
      result |= bytes[bytes.size() - i] << i;
    return result;
  }

  template<template<typename...> typename CONTAINER>
  struct simple_memory
  {
  protected:
    using container_t = CONTAINER<uint8_t>;
    container_t _data;
    size_t _cursor = 0;

    void _write(size_t s)
    {
      if constexpr (lak::is_resizable_v<container_t>)
      {
        if (s > remaining()) _data.resize(size() + s);
      }
      else
      {
        ASSERT_LESS_OR_EQUAL(s, remaining());
      }
    }

  public:
    simple_memory() : _data(), _cursor(0) {}

    simple_memory(const container_t &data, size_t cur = 0) : _data(data)
    {
      seek(cur);
    }

    simple_memory(container_t &&data, size_t cur = 0) : _data(lak::move(data))
    {
      seek(cur);
    }

    simple_memory(const simple_memory &other) = default;

    simple_memory(simple_memory &&other)
    : _data(lak::move(other._data)), _cursor(other._cursor)
    {
      other._cursor = 0;
    }

    simple_memory &operator=(const simple_memory &other) = default;

    simple_memory &operator=(simple_memory &&other)
    {
      lak::swap(_data, other._data);
      _cursor       = other._cursor;
      other._cursor = 0;
      return *this;
    }

    void clear()
    {
      _data   = container_t{};
      _cursor = 0;
    }

    uint8_t *data() { return _data.data(); }
    const uint8_t *data() const { return _data.data(); }

    const container_t &get() const { return _data; }

    size_t size() const { return _data.size(); }

    size_t position() const { return _cursor; }

    size_t remaining() const { return _data.size() - _cursor; }

    uint8_t *begin() { return data(); }
    const uint8_t *begin() const { return data(); }

    uint8_t *end() { return data() + size(); }
    const uint8_t *end() const { return data() + size(); }

    uint8_t *cursor() { return data() + _cursor; }
    const uint8_t *cursor() const { return data() + _cursor; }

    simple_memory &seek(size_t to)
    {
      ASSERT_LESS_OR_EQUAL(to, _data.size());
      _cursor = to;
      return *this;
    }

    simple_memory &skip(size_t amount)
    {
      ASSERT_LESS_OR_EQUAL(amount, _data.size());
      _cursor += amount;
      return *this;
    }

    simple_memory &unread(size_t amount)
    {
      ASSERT_LESS_OR_EQUAL(amount, _cursor);
      _cursor -= amount;
      return *this;
    }

    template<typename T>
    T peek_ne() const
    {
      ASSERT_LESS_OR_EQUAL(sizeof(T), remaining());
      return *reinterpret_cast<const T *>(cursor());
    }
    template<typename T>
    T peek_le() const
    {
      ASSERT_LESS_OR_EQUAL(sizeof(T), remaining());
      return lak::from_le_bytes(lak::span<const uint8_t, sizeof(T)>(cursor()));
    }
    template<typename T>
    T peek_be() const
    {
      ASSERT_LESS_OR_EQUAL(sizeof(T), remaining());
      return lak::from_be_bytes(lak::span<const uint8_t, sizeof(T)>(cursor()));
    }
    template<typename T>
    lak::span<const T> peek_str_ne(size_t max_len = -1) const
    {
      max_len     = std::min(max_len, remaining() / sizeof(T));
      auto *begin = reinterpret_cast<const T *>(cursor());
      auto *end   = begin;
      for (size_t i = 0; i < max_len; ++i, ++end)
        if (*end == T(0)) break;
      return lak::span(begin, end);
    }
    template<typename T>
    lak::string<T> peek_str_le(size_t max_len = -1) const
    {
      max_len = std::min(max_len, remaining() / sizeof(T));
      lak::string<T> result;
      auto *iter = cursor();
      for (size_t i = 0; i < max_len; ++i, iter += sizeof(T))
      {
        T c = lak::from_le_bytes(lak::span<const uint8_t, sizeof(T)>(iter));
        if (c == T(0)) break;
        result.append(c);
      }
      return result;
    }
    template<typename T>
    lak::string<T> peek_str_be(size_t max_len = -1) const
    {
      max_len = std::min(max_len, remaining() / sizeof(T));
      lak::string<T> result;
      auto *iter = cursor();
      for (size_t i = 0; i < max_len; ++i, iter += sizeof(T))
      {
        T c = lak::from_be_bytes(lak::span<const uint8_t, sizeof(T)>(iter));
        if (c == T(0)) break;
        result.append(c);
      }
      return result;
    }
    template<typename T>
    lak::span<const T> peek_fixed_str_ne(size_t len) const
    {
      ASSERT_LESS_OR_EQUAL(len, remaining() / sizeof(T));
      return lak::span<const T>(lak::span(cursor(), sizeof(T)));
    }
    template<typename T>
    lak::string<T> peek_fixed_str_le(size_t len) const
    {
      ASSERT_LESS_OR_EQUAL(len, remaining() / sizeof(T));
      lak::string<T> result;
      auto *iter = cursor();
      for (size_t i = 0; i < len; ++i, iter += sizeof(T))
        result.append(
          lak::from_le_bytes(lak::span<const uint8_t, sizeof(T)>(iter)));
      return result;
    }
    template<typename T>
    lak::string<T> peek_fixed_str_be(size_t len) const
    {
      ASSERT_LESS_OR_EQUAL(len, remaining() / sizeof(T));
      lak::string<T> result;
      auto *iter = cursor();
      for (size_t i = 0; i < len; ++i, iter += sizeof(T))
        result.append(
          lak::from_be_bytes(lak::span<const uint8_t, sizeof(T)>(iter)));
      return result;
    }

    template<typename T>
    T read_ne()
    {
      T result = peek_ne<T>();
      _cursor += sizeof(T);
      return result;
    }
    template<typename T>
    T read_le()
    {
      T result = peek_le<T>();
      _cursor += sizeof(T);
      return result;
    }
    template<typename T>
    T read_be()
    {
      T result = peek_be<T>();
      _cursor += sizeof(T);
      return result;
    }
    template<typename T>
    lak::span<const T> read_str_ne(size_t max_len = -1)
    {
      auto result = peek_str_ne<T>(max_len);
      _cursor += result.size() * sizeof(T);
      return result;
    }
    template<typename T>
    lak::string<T> read_str_le(size_t max_len = -1)
    {
      auto result = peek_str_le<T>(max_len);
      _cursor += result.size() * sizeof(T);
      return result;
    }
    template<typename T>
    lak::string<T> read_str_be(size_t max_len = -1)
    {
      auto result = peek_str_be<T>(max_len);
      _cursor += result.size() * sizeof(T);
      return result;
    }
    template<typename T>
    lak::span<const T> read_fixed_str_ne(size_t len)
    {
      auto result = peek_fixed_str_ne<T>(len);
      _cursor += len * sizeof(T);
      return result;
    }
    template<typename T>
    lak::string<T> read_fixed_str_le(size_t len)
    {
      auto result = peek_fixed_str_le<T>(len);
      _cursor += len * sizeof(T);
      return result;
    }
    template<typename T>
    lak::string<T> read_fixed_str_be(size_t len)
    {
      auto result = peek_fixed_str_be<T>(len);
      _cursor += len * sizeof(T);
      return result;
    }

    template<typename T>
    simple_memory &write_ne(T t)
    {
      _write(sizeof(T));
      *reinterpret_cast<T *>(cursor()) = t;
      _cursor += sizeof(T);
      return *this;
    }
    template<typename T>
    simple_memory &write_le(T t)
    {
      _write(sizeof(T));
      for (size_t i = 0; i < sizeof(T); ++i)
      {
        *cursor() = (t >> i) & 0xFFU;
        ++_cursor;
      }
      _cursor += sizeof(T);
      return *this;
    }
    template<typename T>
    simple_memory &write_be(T t)
    {
      _write(sizeof(T));
      for (size_t i = sizeof(T); i-- > 0;)
      {
        *cursor() = (t >> i) & 0xFFU;
        ++_cursor;
      }
      _cursor += sizeof(T);
      return *this;
    }
    inline simple_memory &write(lak::span<const uint8_t> bytes)
    {
      _write(bytes.size());
      lak::copy(bytes.begin(), bytes.end(), cursor());
      return *this;
    }
    template<typename T>
    simple_memory &write(lak::span<const T> ts)
    {
      return write(lak::span<const uint8_t>(ts));
    }
    template<typename T>
    simple_memory &write_ne(lak::span<const T> ts)
    {
      return write(lak::span<const uint8_t>(ts));
    }
    template<typename T>
    simple_memory &write_le(lak::span<const T> ts)
    {
      for (const auto &t : ts) write_le(t);
      return *this;
    }
    template<typename T>
    simple_memory &write_be(lak::span<const T> ts)
    {
      for (const auto &t : ts) write_be(t);
      return *this;
    }

    inline container_t read(size_t count)
    {
      ASSERT_LESS_OR_EQUAL(count, remaining());
      auto result = container_t(cursor(), cursor() + count);
      _cursor += count;
      return result;
    }
    container_t read(size_t from, size_t to) const
    {
      ASSERT_LESS_OR_EQUAL(from, to);
      ASSERT_LESS_OR_EQUAL(to, size());
      return container_t(data() + from, data() + to);
    }

    inline uint8_t peek_u8() const { return peek_ne<uint8_t>(); }
    inline uint8_t read_u8() { return read_ne<uint8_t>(); }
    inline simple_memory &write_u8(uint8_t v) { return write_ne<uint8_t>(v); }

    inline int8_t peek_s8() const { return peek_ne<int8_t>(); }
    inline int8_t read_s8() { return read_ne<int8_t>(); }
    inline simple_memory &write_s8(int8_t v) { return write_ne<int8_t>(v); }

    inline uint16_t peek_u16() const { return peek_ne<uint16_t>(); }
    inline uint16_t read_u16() { return read_ne<uint16_t>(); }
    inline simple_memory &write_u16(uint16_t v)
    {
      return write_ne<uint16_t>(v);
    }

    inline int16_t peek_s16() const { return peek_ne<int16_t>(); }
    inline int16_t read_s16() { return read_ne<int16_t>(); }
    inline simple_memory &write_s16(int16_t v) { return write_ne<int16_t>(v); }

    inline uint32_t peek_u32() const { return peek_ne<uint32_t>(); }
    inline uint32_t read_u32() { return read_ne<uint32_t>(); }
    inline simple_memory &write_u32(uint32_t v)
    {
      return write_ne<uint32_t>(v);
    }

    inline int32_t peek_s32() const { return peek_ne<int32_t>(); }
    inline int32_t read_s32() { return read_ne<int32_t>(); }
    inline simple_memory &write_s32(int32_t v) { return write_ne<int32_t>(v); }

#if UINTMAX_MAX > 0xFFFFFFFF
    inline uint64_t peek_u64() const { return peek_ne<uint64_t>(); }
    inline uint64_t read_u64() { return read_ne<uint64_t>(); }
    inline simple_memory &write_u64(uint64_t v)
    {
      return write_ne<uint64_t>(v);
    }

    inline int64_t peek_s64() const { return peek_ne<int64_t>(); }
    inline int64_t read_s64() { return read_ne<int64_t>(); }
    inline simple_memory &write_s64(int64_t v) { return write_ne<int64_t>(v); }
#endif

    inline lak::span<const char> peek_astring(size_t max_len = -1) const
    {
      return peek_str_ne<char>(max_len);
    }
    inline lak::span<const char8_t> peek_u8string(size_t max_len = -1) const
    {
      return peek_str_ne<char8_t>(max_len);
    }
    inline lak::span<const char16_t> peek_u16string(size_t max_len = -1) const
    {
      return peek_str_ne<char16_t>(max_len);
    }
    inline lak::span<const char32_t> peek_u32string(size_t max_len = -1) const
    {
      return peek_str_ne<char32_t>(max_len);
    }

    inline lak::span<const char> peek_astring_exact(size_t len) const
    {
      return peek_fixed_str_ne<char>(len);
    }
    inline lak::span<const char8_t> peek_u8string_exact(size_t len) const
    {
      return peek_fixed_str_ne<char8_t>(len);
    }
    inline lak::span<const char16_t> peek_u16string_exact(size_t len) const
    {
      return peek_fixed_str_ne<char16_t>(len);
    }
    inline lak::span<const char32_t> peek_u32string_exact(size_t len) const
    {
      return peek_fixed_str_ne<char32_t>(len);
    }

    inline lak::span<const char> read_astring(size_t max_len = -1)
    {
      return read_str_ne<char>(max_len);
    }
    inline lak::span<const char8_t> read_u8string(size_t max_len = -1)
    {
      return read_str_ne<char8_t>(max_len);
    }
    inline lak::span<const char16_t> read_u16string(size_t max_len = -1)
    {
      return read_str_ne<char16_t>(max_len);
    }
    inline lak::span<const char32_t> read_u32string(size_t max_len = -1)
    {
      return read_str_ne<char32_t>(max_len);
    }

    inline lak::span<const char> read_astring_exact(size_t len)
    {
      return read_fixed_str_ne<char>(len);
    }
    inline lak::span<const char8_t> read_u8string_exact(size_t len)
    {
      return read_fixed_str_ne<char8_t>(len);
    }
    inline lak::span<const char16_t> read_u16string_exact(size_t len)
    {
      return read_fixed_str_ne<char16_t>(len);
    }
    inline lak::span<const char32_t> read_u32string_exact(size_t len)
    {
      return read_fixed_str_ne<char32_t>(len);
    }
  };

  using vector_memory = simple_memory<std::vector>;
  using array_memory  = simple_memory<lak::array>;
  using span_memory   = simple_memory<lak::span>;

  using memory = vector_memory;
}

#endif // LAK_MEMORY_HPP
