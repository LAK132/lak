#ifndef LAK_MEMORY_HPP
#define LAK_MEMORY_HPP

#include "string.hpp"

#include <stdint.h>
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
  struct memory
  {
  protected:
    std::vector<uint8_t> _data = {};

  public:
    size_t position = 0;
    // NATIVE: pointer cast
    // LITTLE: most significant byte last
    // BIG: most significant byte first
    enum class endian_t : uint8_t
    {
      NATIVE,
      LITTLE,
      BIG
    };
    endian_t endian = endian_t::NATIVE;

    memory() = default;
    memory(const std::vector<uint8_t> &mem, size_t pos = 0);
    memory(const memory &other);
    memory &operator=(const std::vector<uint8_t> &mem);
    memory &operator=(const memory &other);

    auto begin() -> decltype(_data.begin());
    auto begin() const -> decltype(_data.begin());
    auto end() -> decltype(_data.end());
    auto end() const -> decltype(_data.end());
    auto cursor() -> decltype(_data.begin());
    auto cursor() const -> decltype(_data.begin());

    inline operator const std::vector<uint8_t> &() const { return _data; }

    size_t size() const;
    size_t remaining() const;
    uint8_t *data();
    const uint8_t *data() const;
    uint8_t &operator[](size_t index);
    const uint8_t &operator[](size_t index) const;

    memory &reserve(size_t sz);
    memory &insert(size_t count);
    memory &insert(size_t count, uint8_t fill);
    memory &clear();
    size_t pos() const;
    memory &seek(size_t to);
    memory &skip(size_t amount);
    uint8_t *get();
    const uint8_t *get() const;

    std::vector<uint8_t> read(size_t count);
    std::vector<uint8_t> read(size_t from, size_t count) const;
    std::vector<uint8_t> read_range(size_t mark) const;
    std::vector<uint8_t> read_range(size_t from, size_t to) const;
    memory &write(const std::vector<uint8_t> &bytes);
    memory &write(const memory &other);

    // ASCII
    std::string read_string(size_t max_len = SIZE_MAX);
    std::string read_string_exact(size_t exact_len);
    std::string peek_string(size_t max_len = SIZE_MAX) const;
    memory &write_string(const std::string &str, bool terminate = false);

    // UTF-8
    std::u8string read_u8string(size_t max_len = SIZE_MAX);
    std::u8string read_u8string_exact(size_t exact_len);
    std::u8string peek_u8string(size_t max_len = SIZE_MAX) const;
    memory &write_u8string(const std::u8string &str, bool terminate = false);

    // UTF-16
    std::u16string read_u16string(size_t max_len = SIZE_MAX);
    std::u16string read_u16string_exact(size_t exact_len);
    std::u16string peek_u16string(size_t max_len = SIZE_MAX) const;
    memory &write_u16string(const std::u16string &str, bool terminate = false);

    // UTF-32
    std::u32string read_u32string(size_t max_len = SIZE_MAX);
    std::u32string read_u32string_exact(size_t exact_len);
    std::u32string peek_u32string(size_t max_len = SIZE_MAX) const;
    memory &write_u32string(const std::u32string &str, bool terminate = false);

    uint8_t read_u8(const uint8_t def = 0);
    uint8_t peek_u8(const uint8_t def = 0) const;
    memory &write_u8(const uint8_t v);

    int8_t read_s8(const int8_t def = 0);
    int8_t peek_s8(const int8_t def = 0) const;
    memory &write_s8(const int8_t v);

    uint16_t read_u16(const uint16_t def = 0);
    uint16_t peek_u16(const uint16_t def = 0) const;
    memory &write_u16(const uint16_t v);

    int16_t read_s16(const int16_t def = 0);
    int16_t peek_s16(const int16_t def = 0) const;
    memory &write_s16(const int16_t v);

    uint32_t read_u32(const uint32_t def = 0);
    uint32_t peek_u32(const uint32_t def = 0) const;
    memory &write_u32(const uint32_t v);

    int32_t read_s32(const int32_t def = 0);
    int32_t peek_s32(const int32_t def = 0) const;
    memory &write_s32(const int32_t v);

#if UINTMAX_MAX > 0xFFFFFFFF
    uint64_t read_u64(const uint64_t def = 0);
    uint64_t peek_u64(const uint64_t def = 0) const;
    memory &write_u64(const uint64_t v);

    int64_t read_s64(const int64_t def = 0);
    int64_t peek_s64(const int64_t def = 0) const;
    memory &write_s64(const uint64_t v);
#endif
  };
}

#endif // LAK_MEMORY_HPP
