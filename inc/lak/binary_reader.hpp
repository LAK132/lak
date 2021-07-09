#ifndef LAK_BINARY_READER_HPP
#define LAK_BINARY_READER_HPP

#include "lak/array.hpp"
#include "lak/endian.hpp"
#include "lak/result.hpp"
#include "lak/span.hpp"
#include "lak/stdint.hpp"

#include "lak/debug.hpp"

namespace lak
{
  template<typename T, lak::endian E>
  struct from_bytes_traits
  {
    using value_type = lak::nonesuch;

    static constexpr size_t size = 0;
  };

  template<typename T, lak::endian E>
  static constexpr bool has_from_bytes_traits =
    !lak::is_same_v<typename lak::from_bytes_traits<T, E>::value_type,
                    lak::nonesuch>;

  template<typename T, lak::endian E = lak::endian::little>
  static constexpr size_t from_bytes_size_v =
    lak::from_bytes_traits<T, E>::size;

  template<typename T, lak::endian E = lak::endian::little>
  using from_bytes_value_type_t =
    typename lak::from_bytes_traits<T, E>::value_type;

  template<typename T, lak::endian E = lak::endian::little>
  T from_bytes(lak::span<const uint8_t, lak::from_bytes_size_v<T, E>> bytes);

  template<typename T, lak::endian E = lak::endian::little>
  lak::result<T> from_bytes(lak::span<const uint8_t> bytes);

  template<typename T, lak::endian E = lak::endian::little>
  lak::result<lak::array<T>> array_from_bytes(lak::span<const uint8_t> bytes,
                                              size_t count);

  struct binary_reader
  {
    lak::span<const uint8_t> data = {};
    size_t cursor                 = 0;

    binary_reader() = default;
    binary_reader(lak::span<const uint8_t> bytes) : data(bytes), cursor(0) {}
    binary_reader(const binary_reader &) = default;
    binary_reader &operator=(const binary_reader &) = default;

    inline lak::span<const uint8_t> remaining() const
    {
      return data.subspan(cursor);
    }
    inline bool empty() const { return cursor >= data.size(); }
    inline size_t position() const { return cursor; }
    inline size_t size() const { return data.size(); }
    inline lak::result<> seek(size_t pos)
    {
      if (pos > data.size()) return lak::err_t{};
      cursor = pos;
      return lak::ok_t{};
    }
    inline lak::result<> skip(size_t count)
    {
      if (cursor + count > data.size()) return lak::err_t{};
      cursor += count;
      return lak::ok_t{};
    }
    inline lak::result<> unread(size_t count)
    {
      if (count > cursor) return lak::err_t{};
      cursor -= count;
      return lak::ok_t{};
    }

    template<typename T, lak::endian E = lak::endian::little>
    lak::result<T> peek()
    {
      return lak::from_bytes<T, E>(remaining());
    }

    template<typename T, lak::endian E = lak::endian::little>
    lak::result<T> read()
    {
      return peek<T, E>().if_ok(
        [&](...) { this->cursor += lak::from_bytes_size_v<T, E>; });
    }

    template<typename T, lak::endian E = lak::endian::little>
    lak::result<lak::array<T>> peek(size_t count)
    {
      return lak::array_from_bytes<T, E>(remaining(), count);
    }

    template<typename T, lak::endian E = lak::endian::little>
    lak::result<lak::array<T>> read(size_t count)
    {
      return peek<T, E>(count).if_ok(
        [&](...) { this->cursor += lak::from_bytes_size_v<T, E> * count; });
    }

    // always read count bytes even if the c string isn't the full count bytes
    template<typename CHAR, lak::endian E = lak::endian::little>
    lak::result<lak::string<CHAR>> read_exact_c_str(size_t count)
    {
      return read<CHAR>(count).map(
        [](lak::array<CHAR> &&array)
        {
          size_t len = 0;
          for (const auto &c : array)
          {
            if (c == CHAR(0)) break;
            ++len;
          }
          return lak::string<CHAR>(array.data(), array.data() + len);
        });
    }

    template<typename CHAR, lak::endian E = lak::endian::little>
    lak::result<lak::string<CHAR>> read_c_str(size_t max_size = SIZE_MAX)
    {
      lak::string<CHAR> result;
      const size_t old_cursor = cursor;
      while (max_size-- > 0ULL)
      {
        const auto r = read<CHAR, E>();
        if (r.if_err([](...) { ERROR("read failed"); }).is_err()) break;
        const CHAR c = r.unsafe_unwrap();
        if (c == 0) return lak::ok_t{lak::move(result)};
        result += c;
      }
      cursor = old_cursor;
      if (max_size == SIZE_MAX) ERROR("max size reached");
      return lak::err_t{};
    }

    template<typename CHAR, lak::endian E = lak::endian::little>
    lak::string<CHAR> read_any_c_str(size_t max_size = SIZE_MAX)
    {
      lak::string<CHAR> result;
      while (max_size-- > 0ULL)
      {
        const auto r = read<CHAR, E>();
        if (r.is_err()) break;
        const CHAR c = r.unsafe_unwrap();
        if (c == 0) break;
        result += c;
      }
      return result;
    }

#define BINARY_READER_MEMBERS(TYPE, NAME, ...)                                \
  template<lak::endian E = lak::endian::little>                               \
  inline lak::result<TYPE> peek_##NAME()                                      \
  {                                                                           \
    return peek<TYPE, E>();                                                   \
  }                                                                           \
  template<lak::endian E = lak::endian::little>                               \
  inline lak::result<TYPE> read_##NAME()                                      \
  {                                                                           \
    return read<TYPE, E>();                                                   \
  }
    LAK_FOREACH_INTEGER(BINARY_READER_MEMBERS)
    LAK_FOREACH_FLOAT(BINARY_READER_MEMBERS)
#undef BINARY_READER_MEMBERS
  };
}

#include "lak/binary_reader.inl"

#endif
