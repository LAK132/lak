#ifndef LAK_IMAGE_HPP
#define LAK_IMAGE_HPP

#include "lak/vec.hpp"

#include <cstring>
#include <vector>

namespace lak
{
  template<typename T>
  struct image
  {
    using size_type  = vec2s_t;
    using value_type = T;

    image() = default;
    image(size_type size) { resize(size); }

    image(const image &) = default;
    image &operator=(const image &) = default;

    image(image &&img) : _size(img._size), _value(std::move(img._value)) {}
    image &operator=(image &&img)
    {
      _size  = img._size;
      _value = std::move(img._value);
    }

    value_type &operator[](const size_type index)
    {
      return _value[index.x + (_size.x * index.y)];
    }

    const value_type &operator[](const size_type index) const
    {
      return _value[index.x + (_size.x * index.y)];
    }

    value_type &operator[](const size_t index) { return _value[index]; }

    const value_type &operator[](const size_t index) const
    {
      return _value[index];
    }

    void resize(const size_type sz)
    {
      _size = sz;
      _value.resize(sz.x * sz.y);
    }

    size_type size() const { return _size; }

    size_t contig_size() const { return _value.size(); }

    value_type *data() { return _value.data(); }

    const value_type *data() const { return _value.data(); }

    void fill(const value_type &value)
    {
      for (auto &v : _value) v = value;
    }

  private:
    size_type _size;
    std::vector<value_type> _value;
  };

  using image3_t = image<color3_t>;
  using image4_t = image<color4_t>;

  image3_t ImageFromRGB24(uint8_t *pixels, vec2s_t size)
  {
    image3_t result;
    result.resize(size);
    std::memcpy(&result[0].x, pixels, size.x * size.y * 3);
    return result;
  }

  image4_t ImageFromRGBA32(uint8_t *pixels, vec2s_t size)
  {
    image4_t result;
    result.resize(size);
    std::memcpy(&result[0].x, pixels, size.x * size.y * 4);
    return result;
  }
}

#endif
