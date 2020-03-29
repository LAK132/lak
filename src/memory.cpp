#include "lak/memory.hpp"

#include <algorithm>
#include <cstring>

namespace lak
{
  namespace
  {
    namespace impl
    {
      template<typename INT>
      INT read_int(
        const uint8_t *mem,
        const memory::endian_t endian,
        size_t *position,
        const INT def = 0)
      {
        if (mem == nullptr) return def;

        if (position != nullptr) *position += sizeof(INT);

        if (endian == memory::endian_t::NATIVE)
        {
          return *reinterpret_cast<const INT *>(mem);
        }
        else
        {
          INT result = 0;
          if (endian == memory::endian_t::LITTLE)
            for (size_t i = 0; i < sizeof(INT); ++i)
              result |= static_cast<INT>(mem[i]) << (8 * i);
          else
            for (size_t i = 0; i < sizeof(INT); ++i)
              result |= static_cast<INT>(mem[i])
                        << (8 * (sizeof(INT) - (i + 1)));
          return result;
        }
      }

      template<typename CHAR>
      std::basic_string<CHAR> read_string(
        const uint8_t *begin,
        const uint8_t *end,
        const memory::endian_t endian,
        size_t *position)
      {
        if (begin == nullptr || end == nullptr || begin == end)
          return std::basic_string<CHAR>();

        const CHAR *_begin = reinterpret_cast<const CHAR *>(begin);
        const CHAR *_end   = reinterpret_cast<const CHAR *>(end);
        const CHAR *_end2  = std::find(_begin, _end, 0);
        end                = reinterpret_cast<const uint8_t *>(_end2);

        if (position != nullptr)
        {
          *position += static_cast<size_t>(end - begin);
          if (_end2 < _end && *_end2 == 0) *position += sizeof(CHAR);
        }

        _end = _end2;

        if (endian == memory::endian_t::NATIVE)
        {
          return std::basic_string<CHAR>(_begin, _end);
        }
        else
        {
          std::basic_string<CHAR> result;
          result.reserve(static_cast<size_t>(_end - _begin));
          for (auto it = _begin; it != _end; ++it)
            result += read_int<CHAR>(
              reinterpret_cast<const uint8_t *>(it), endian, nullptr, 0);
          return result;
        }
      }

      template<typename INT>
      void write_int(
        uint8_t *mem,
        const INT val,
        const memory::endian_t endian,
        size_t *position)
      {
        if (mem == nullptr) return;

        if (position) *position += sizeof(INT);

        if (endian == memory::endian_t::NATIVE)
        {
          *reinterpret_cast<INT *>(mem) = val;
        }
        else
        {
          if (endian == memory::endian_t::LITTLE)
            for (size_t i = 0; i < sizeof(INT); ++i)
              mem[i] = static_cast<uint8_t>(val >> (8 * i));
          else
            for (size_t i = 0; i < sizeof(INT); ++i)
              mem[i] =
                static_cast<uint8_t>(val >> (8 * (sizeof(INT) - (i + 1))));
        }
      }

      template<typename CHAR>
      void write_string(
        uint8_t *mem,
        const std::basic_string<CHAR> str,
        const bool terminate,
        const memory::endian_t endian,
        size_t *position)
      {
        if (mem == nullptr) return;

        if (endian == memory::endian_t::NATIVE)
        {
          std::memcpy(mem, str.c_str(), str.size() * sizeof(CHAR));
          if (position) *position += str.size() * sizeof(CHAR);
          if (terminate) write_int(mem + str.size(), 0, endian, position);
        }
        else
        {
          for (const auto c : str) write_int(mem++, c, endian, position);
          if (terminate) write_int(mem, 0, endian, position);
        }
      }
    }
  }

  memory::memory(const std::vector<uint8_t> &mem, size_t pos)
  : _data(mem), position(pos)
  {
  }

  memory::memory(const memory &other)
  : _data(other._data), position(other.position), endian(other.endian)
  {
  }

  memory &memory::operator=(const std::vector<uint8_t> &mem)
  {
    _data    = mem;
    position = 0;
    return *this;
  }

  memory &memory::operator=(const memory &other)
  {
    _data    = other._data;
    position = other.position;
    endian   = other.endian;
    return *this;
  }

  auto memory::begin() -> decltype(_data.begin()) { return _data.begin(); }

  auto memory::begin() const -> decltype(_data.begin())
  {
    return _data.begin();
  }

  auto memory::end() -> decltype(_data.end()) { return _data.end(); }

  auto memory::end() const -> decltype(_data.end()) { return _data.end(); }

  auto memory::cursor() -> decltype(_data.begin())
  {
    if (position < _data.size()) return _data.begin() + position;
    return _data.end();
  }

  auto memory::cursor() const -> decltype(_data.begin())
  {
    if (position < _data.size()) return _data.begin() + position;
    return _data.end();
  }

  size_t memory::size() const { return _data.size(); }

  size_t memory::remaining() const
  {
    if (position < _data.size()) return _data.size() - position;
    return 0;
  }

  uint8_t *memory::data() { return _data.data(); }

  const uint8_t *memory::data() const { return _data.data(); }

  uint8_t &memory::operator[](size_t index) { return _data[index]; }

  const uint8_t &memory::operator[](size_t index) const
  {
    return _data[index];
  }

  memory &memory::reserve(size_t sz)
  {
    if (sz > _data.capacity()) _data.reserve(sz);
    return *this;
  }

  memory &memory::insert(size_t count)
  {
    const size_t old_end = _data.size();
    _data.resize(_data.size() + count);
    if (position < old_end)
    {
      // we need to move the _data at the end
      std::memmove(get() + count, get(), old_end - position);
    }
    return *this;
  }

  memory &memory::insert(size_t count, uint8_t fill)
  {
    insert(count);
    std::memset(get(), fill, count);
    return *this;
  }

  memory &memory::clear()
  {
    _data.clear();
    return *this;
  }

  size_t memory::pos() const { return position; }

  memory &memory::seek(size_t to)
  {
    position = to;
    return *this;
  }

  memory &memory::skip(size_t amount)
  {
    position += amount;
    return *this;
  }

  uint8_t *memory::get()
  {
    if (position < _data.size()) return _data.data() + position;
    return nullptr;
  }

  const uint8_t *memory::get() const
  {
    if (position < _data.size()) return _data.data() + position;
    return nullptr;
  }

  std::vector<uint8_t> memory::read(size_t count)
  {
    const auto _begin = cursor();
    const auto _end   = _begin + std::min(count, remaining());
    position += count;
    return std::vector<uint8_t>(_begin, _end);
  }

  std::vector<uint8_t> memory::read(size_t from, size_t count) const
  {
    const auto _begin = begin() + std::min(from, _data.size());
    const auto _end   = begin() + std::min(from + count, _data.size());
    return std::vector<uint8_t>(_begin, _end);
  }

  std::vector<uint8_t> memory::read_range(size_t mark) const
  {
    const auto _begin =
      begin() + std::min(std::min(mark, position), _data.size());
    const auto _end =
      begin() + std::min(std::max(mark, position), _data.size());
    return std::vector<uint8_t>(_begin, _end);
  }

  std::vector<uint8_t> memory::read_range(size_t from, size_t to) const
  {
    const auto _begin = begin() + std::min(from, _data.size());
    const auto _end   = begin() + std::min(to, _data.size());
    return std::vector<uint8_t>(_begin, _end);
  }

  memory &memory::write(const std::vector<uint8_t> &bytes)
  {
    insert(bytes.size());
    std::memcpy(get(), bytes.data(), bytes.size());
    position += bytes.size();
    return *this;
  }

  memory &memory::write(const memory &other) { return write(other._data); }

  std::string memory::read_string(size_t max_len)
  {
    const uint8_t *_begin = get();
    if (_begin == nullptr) return std::string();
    const uint8_t *_end = _begin;
    if (max_len < SIZE_MAX / sizeof(char))
      _end += std::min(max_len * sizeof(char), remaining());
    else
      _end += remaining();
    return impl::read_string<char>(_begin, _end, endian, &position);
  }

  std::string memory::read_string_exact(size_t exact_len)
  {
    const uint8_t *_begin = get();
    position += exact_len * sizeof(char);
    if (_begin == nullptr) return std::string();
    const uint8_t *_end =
      _begin + std::min(exact_len * sizeof(char), remaining());
    return impl::read_string<char>(_begin, _end, endian, nullptr);
  }

  std::string memory::peek_string(size_t max_len) const
  {
    const uint8_t *_begin = get();
    if (_begin == nullptr) return std::string();
    const uint8_t *_end = _begin;
    if (max_len < SIZE_MAX / sizeof(char))
      _end += std::min(max_len * sizeof(char), remaining());
    else
      _end += remaining();
    return impl::read_string<char>(_begin, _end, endian, nullptr);
  }

  memory &memory::write_string(const std::string &str, bool terminate)
  {
    const size_t str_len = (str.size() + (terminate ? 1 : 0)) * sizeof(char);
    insert(str_len);
    impl::write_string(get(), str, terminate, endian, &position);
    return *this;
  }

  std::u8string memory::read_u8string(size_t max_len)
  {
    const uint8_t *_begin = get();
    if (_begin == nullptr) return std::u8string();
    const uint8_t *_end = _begin;
    if (max_len < SIZE_MAX / sizeof(char8_t))
      _end += std::min(max_len * sizeof(char8_t), remaining());
    else
      _end += remaining();
    return impl::read_string<char8_t>(_begin, _end, endian, &position);
  }

  std::u8string memory::read_u8string_exact(size_t exact_len)
  {
    const uint8_t *_begin = get();
    position += exact_len * sizeof(char8_t);
    if (_begin == nullptr) return std::u8string();
    const uint8_t *_end =
      _begin + std::min(exact_len * sizeof(char), remaining());
    return impl::read_string<char8_t>(_begin, _end, endian, nullptr);
  }

  std::u8string memory::peek_u8string(size_t max_len) const
  {
    const uint8_t *_begin = get();
    if (_begin == nullptr) return std::u8string();
    const uint8_t *_end = _begin;
    if (max_len < SIZE_MAX / sizeof(char8_t))
      _end += std::min(max_len * sizeof(char8_t), remaining());
    else
      _end += remaining();
    return impl::read_string<char8_t>(_begin, _end, endian, nullptr);
  }

  memory &memory::write_u8string(const std::u8string &str, bool terminate)
  {
    const size_t str_len =
      (str.size() + (terminate ? 1 : 0)) * sizeof(char8_t);
    insert(str_len);
    impl::write_string(get(), str, terminate, endian, &position);
    return *this;
  }

  std::u16string memory::read_u16string(size_t max_len)
  {
    const uint8_t *_begin = get();
    if (_begin == nullptr) return std::u16string();
    const uint8_t *_end = _begin;
    if (max_len < SIZE_MAX / sizeof(char16_t))
      _end += std::min(max_len * sizeof(char16_t), remaining());
    else
      _end += remaining();
    return impl::read_string<char16_t>(_begin, _end, endian, &position);
  }

  std::u16string memory::read_u16string_exact(size_t exact_len)
  {
    const uint8_t *_begin = get();
    position += exact_len * sizeof(char16_t);
    if (_begin == nullptr) return std::u16string();
    const uint8_t *_end =
      _begin + std::min(exact_len * sizeof(char16_t), remaining());
    return impl::read_string<char16_t>(_begin, _end, endian, nullptr);
  }

  std::u16string memory::peek_u16string(size_t max_len) const
  {
    const uint8_t *_begin = get();
    if (_begin == nullptr) return std::u16string();
    const uint8_t *_end = _begin;
    if (max_len < SIZE_MAX / sizeof(char16_t))
      _end += std::min(max_len * sizeof(char16_t), remaining());
    else
      _end += remaining();
    return impl::read_string<char16_t>(_begin, _end, endian, nullptr);
  }

  memory &memory::write_u16string(const std::u16string &str, bool terminate)
  {
    const size_t str_len =
      (str.size() + (terminate ? 1 : 0)) * sizeof(char16_t);
    insert(str_len);
    impl::write_string(get(), str, terminate, endian, &position);
    return *this;
  }

  std::u32string memory::read_u32string(size_t max_len)
  {
    const uint8_t *_begin = get();
    if (_begin == nullptr) return std::u32string();
    const uint8_t *_end = _begin;
    if (max_len < SIZE_MAX / sizeof(char32_t))
      _end += std::min(max_len * sizeof(char32_t), remaining());
    else
      _end += remaining();
    return impl::read_string<char32_t>(_begin, _end, endian, &position);
  }

  std::u32string memory::read_u32string_exact(size_t exact_len)
  {
    const uint8_t *_begin = get();
    position += exact_len * sizeof(char32_t);
    if (_begin == nullptr) return std::u32string();
    const uint8_t *_end =
      _begin + std::min(exact_len * sizeof(char32_t), remaining());
    return impl::read_string<char32_t>(_begin, _end, endian, nullptr);
  }

  std::u32string memory::peek_u32string(size_t max_len) const
  {
    const uint8_t *_begin = get();
    if (_begin == nullptr) return std::u32string();
    const uint8_t *_end = _begin;
    if (max_len < SIZE_MAX / sizeof(char32_t))
      _end += std::min(max_len * sizeof(char32_t), remaining());
    else
      _end += remaining();
    return impl::read_string<char32_t>(_begin, _end, endian, nullptr);
  }

  memory &memory::write_u32string(const std::u32string &str, bool terminate)
  {
    const size_t str_len =
      (str.size() + (terminate ? 1 : 0)) * sizeof(char32_t);
    insert(str_len);
    impl::write_string(get(), str, terminate, endian, &position);
    return *this;
  }

  uint8_t memory::read_u8(const uint8_t def)
  {
    return impl::read_int<uint8_t>(get(), endian, &position, def);
  }

  uint8_t memory::peek_u8(const uint8_t def) const
  {
    return impl::read_int<uint8_t>(get(), endian, nullptr, def);
  }

  memory &memory::write_u8(const uint8_t v)
  {
    insert(sizeof(v));
    impl::write_int(get(), v, endian, &position);
    return *this;
  }

  int8_t memory::read_s8(const int8_t def)
  {
    return impl::read_int<int8_t>(get(), endian, &position, def);
  }

  int8_t memory::peek_s8(const int8_t def) const
  {
    return impl::read_int<int8_t>(get(), endian, nullptr, def);
  }

  memory &memory::write_s8(const int8_t v)
  {
    insert(sizeof(v));
    impl::write_int(get(), v, endian, &position);
    return *this;
  }

  uint16_t memory::read_u16(const uint16_t def)
  {
    return impl::read_int<uint16_t>(get(), endian, &position, def);
  }

  uint16_t memory::peek_u16(const uint16_t def) const
  {
    return impl::read_int<uint16_t>(get(), endian, nullptr, def);
  }

  memory &memory::write_u16(const uint16_t v)
  {
    insert(sizeof(v));
    impl::write_int(get(), v, endian, &position);
    return *this;
  }

  int16_t memory::read_s16(const int16_t def)
  {
    return impl::read_int<int16_t>(get(), endian, &position, def);
  }

  int16_t memory::peek_s16(const int16_t def) const
  {
    return impl::read_int<int16_t>(get(), endian, nullptr, def);
  }

  memory &memory::write_s16(const int16_t v)
  {
    insert(sizeof(v));
    impl::write_int(get(), v, endian, &position);
    return *this;
  }

  uint32_t memory::read_u32(const uint32_t def)
  {
    return impl::read_int<uint32_t>(get(), endian, &position, def);
  }

  uint32_t memory::peek_u32(const uint32_t def) const
  {
    return impl::read_int<uint32_t>(get(), endian, nullptr, def);
  }

  memory &memory::write_u32(const uint32_t v)
  {
    insert(sizeof(v));
    impl::write_int(get(), v, endian, &position);
    return *this;
  }

  int32_t memory::read_s32(const int32_t def)
  {
    return impl::read_int<int32_t>(get(), endian, &position, def);
  }

  int32_t memory::peek_s32(const int32_t def) const
  {
    return impl::read_int<int32_t>(get(), endian, nullptr, def);
  }

  memory &memory::write_s32(const int32_t v)
  {
    insert(sizeof(v));
    impl::write_int(get(), v, endian, &position);
    return *this;
  }

#if UINTMAX_MAX > 0xFFFFFFFF
  uint64_t memory::read_u64(const uint64_t def)
  {
    return impl::read_int<uint64_t>(get(), endian, &position, def);
  }

  uint64_t memory::peek_u64(const uint64_t def) const
  {
    return impl::read_int<uint64_t>(get(), endian, nullptr, def);
  }

  memory &memory::write_u64(const uint64_t v)
  {
    insert(sizeof(v));
    impl::write_int(get(), v, endian, &position);
    return *this;
  }

  int64_t memory::read_s64(const int64_t def)
  {
    return impl::read_int<int64_t>(get(), endian, &position, def);
  }

  int64_t memory::peek_s64(const int64_t def) const
  {
    return impl::read_int<int64_t>(get(), endian, nullptr, def);
  }

  memory &memory::write_s64(const uint64_t v)
  {
    insert(sizeof(v));
    impl::write_int(get(), v, endian, &position);
    return *this;
  }
#endif
}
