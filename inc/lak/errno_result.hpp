#ifndef LAK_ERRNO_RESULT_HPP
#define LAK_ERRNO_RESULT_HPP

#include "lak/result.hpp"
#include "lak/strconv.hpp"
#include "lak/streamify.hpp"

#include <cerrno>
#include <cstring>
#include <ostream>

namespace lak
{
  struct errno_error
  {
    int value;

    static errno_error last_error() { return {errno}; }

    inline lak::astring to_string() const
    {
      return lak::streamify<char>(value, ": ", std::strerror(value));
    }
  };

  template<typename T>
  using errno_result = lak::result<T, errno_error>;
}

[[maybe_unused]] static std::ostream &operator<<(std::ostream &strm,
                                                 const lak::errno_error &err)
{
  return strm << err.to_string();
}

#endif