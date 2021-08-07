#include "lak/strcast.hpp"
#include "lak/strconv.hpp"
#include "lak/string_view.hpp"
#include "lak/type_traits.hpp"

#include <ios>

template<typename... ARGS>
lak::u8string lak::streamify(const ARGS &...args)
{
  std::stringstream _strm;
  auto &strm = static_cast<std::ostream &>(_strm);
  strm << std::hex << std::noshowbase << std::uppercase << std::boolalpha;

  [[maybe_unused]] auto streamer = [&strm](const auto &arg)
  {
    using arg_t = lak::remove_cvref_t<decltype(arg)>;

    if constexpr (lak::is_same_v<arg_t, bool>)
    {
      strm << (arg ? "true" : "false");
    }
    else if constexpr ((std::is_integral_v<arg_t> &&
                        !std::is_same_v<arg_t, char>) ||
                       std::is_enum_v<arg_t>)
    {
      // :TODO: work out if it's possible to check if there is a stream
      // operator overloaded for arg_t enums.
      strm << "0x" << static_cast<uintmax_t>(arg);
    }
    else
    {
      if constexpr (lak::is_span_v<arg_t>)
        ::operator<<(strm, arg);
      else if constexpr (std::is_null_pointer_v<arg_t>)
        strm << "nullptr";
      else if constexpr (lak::is_string_v<lak::remove_cvref_t<arg_t>>)
      {
        using char_type = lak::remove_cvref_t<decltype(arg[0])>;
        if constexpr (lak::is_same_v<char, char_type>)
        {
          // no encoding conversion required
          if constexpr (lak::is_pointer_v<arg_t>)
            strm << lak::string_view<char>::from_c_str(arg);
          else
            strm << lak::string_view(arg);
        }
        else if constexpr (lak::is_same_v<char8_t, char_type>)
        {
          // no encoding conversion required
          if constexpr (lak::is_pointer_v<arg_t>)
            strm << lak::string_view<char8_t>::from_c_str(arg);
          else
            strm << lak::string_view(arg);
        }
        else
        {
          // encoding conversion required
          if constexpr (lak::is_pointer_v<arg_t>)
            strm << lak::strconv<char8_t>(
              lak::string_view<lak::remove_const_t<
                lak::remove_pointer_t<arg_t>>>::from_c_str(arg));
          else
            strm << lak::strconv<char8_t>(lak::string_view(arg));
        }
      }
      else
        strm << arg;
    }
  };

  (streamer(args), ...);

  return lak::as_u8string(_strm.str()).to_string();
}
