#ifndef LAK_ERRNO_RESULT_HPP
#define LAK_ERRNO_RESULT_HPP

#include "lak/result.hpp"
#include "lak/strconv.hpp"
#include "lak/streamify.hpp"

#include <cerrno>
#include <ostream>
#include <system_error>

namespace lak
{
  struct errno_error
  {
    int value;

    static errno_error last_error() { return {errno}; }

    inline lak::astring to_string() const
    {
      return lak::to_astring(lak::streamify(
        value,
        ": ",
        std::error_code(value, std::generic_category()).message()));
    }
  };

  template<typename T = lak::monostate>
  using errno_result = lak::result<T, lak::errno_error>;
}

[[maybe_unused]] inline std::ostream &operator<<(std::ostream &strm,
                                                 const lak::errno_error &err)
{
  return strm << err.to_string();
}

#endif