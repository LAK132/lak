#include "lak/strcast.hpp"
#include "lak/strconv.hpp"
#include "lak/type_traits.hpp"
#include "lak/unicode.hpp"

template<typename CHAR, typename... ARGS>
lak::string<CHAR> lak::streamify(const ARGS &... args)
{
#if 1 // ndef LAK_COMPILER_CPP20
  std::basic_stringstream<
    std::conditional_t<std::is_same_v<CHAR, char8_t>, char, CHAR>>
    _strm;
  auto &strm = static_cast<std::basic_ostream<
    std::conditional_t<std::is_same_v<CHAR, char8_t>, char, CHAR>> &>(_strm);
#else
  std::basic_stringstream<CHAR> _strm;
  auto &strm = static_cast<std::basic_ostream<CHAR> &>(_strm);
#endif
  strm << std::hex << std::noshowbase << std::uppercase << std::boolalpha;

  [[maybe_unused]] auto streamer = [&strm](const auto &arg) {
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
#if 1 // ndef LAK_COMPILER_CPP20
      else if constexpr (lak::is_same_v<const char8_t *,
                                        lak::add_wconst_t<arg_t>> &&
                         std::is_same_v<CHAR, char>)
        strm << lak::as_astring(lak::string_view(arg));
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

#if 1 // ndef LAK_COMPILER_CPP20
  if constexpr (std::is_same_v<CHAR, char8_t>)
    return lak::as_u8string(_strm.str()).to_string();
  else
#endif
    return _strm.str();
}
