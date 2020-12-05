#ifndef LAK_ERRNO_RESULT_HPP
#define LAK_ERRNO_RESULT_HPP

#include "lak/result.hpp"
#include "lak/strconv.hpp"

#include <cerrno>
#include <ostream>

namespace lak
{
  struct errno_error
  {
    int value;

    static errno_error last_error() { return {errno}; }

    inline lak::u8string to_string() const
    {
      return lak::to_u8string(std::strerror(value));
    }
  };

  template<typename T>
  using errno_result = lak::result<T, errno_error>;
}

static std::ostream &operator<<(std::ostream &strm,
                                const lak::errno_error &err)
{
  return strm << std::strerror(err.value);
}

#endif