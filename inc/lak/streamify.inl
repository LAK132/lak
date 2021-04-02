#include "lak/span.hpp"
#include "lak/strconv.hpp"
#include "lak/type_traits.hpp"
#include "lak/unicode.hpp"

template<typename CHAR, typename... ARGS>
lak::string<CHAR> lak::streamify(const ARGS &... args)
{
#if __cplusplus <= 201703L
  std::basic_stringstream<
    std::conditional_t<std::is_same_v<CHAR, char8_t>, char, CHAR>>
    strm;
#else
  std::basic_stringstream<CHAR> strm;
#endif
  strm << std::hex << std::noshowbase << std::uppercase << std::boolalpha;

  auto streamer = [&strm](const auto &arg) {
    using arg_t = lak::remove_cvref_t<decltype(arg)>;

    if constexpr (std::is_integral_v<arg_t> && !std::is_same_v<arg_t, char>)
    {
      strm << "0x" << static_cast<uintmax_t>(arg);
    }
    else
    {
      if constexpr (lak::is_span_v<arg_t>)
        ::operator<<(strm, arg);
      else if constexpr (std::is_null_pointer_v<arg_t>)
        strm << "nullptr";
#if __cplusplus <= 201703L
      else if constexpr (std::is_same_v<arg_t, lak::u8string> &&
                         std::is_same_v<CHAR, char8_t>)
        strm << lak::as_astring(arg);
      else if constexpr (lak::is_string_v<arg_t> &&
                         !std::is_same_v<arg_t, lak::u8string> &&
                         std::is_same_v<CHAR, char8_t>)
        strm << lak::as_astring(lak::to_u8string(arg));
#endif
      else if constexpr (lak::is_string_v<arg_t> &&
                         !std::is_same_v<arg_t, lak::string<CHAR>>)
        strm << lak::strconv<CHAR>(arg);
      else
        strm << arg;
    }
  };

  (streamer(args), ...);

#if __cplusplus <= 201703L
  if constexpr (std::is_same_v<CHAR, char8_t>)
    return lak::as_u8string(strm.str()).to_string();
  else
#endif
    return strm.str();
}
