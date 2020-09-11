namespace lak
{
  template<typename CHAR>
  lak::span<CHAR> string_view(CHAR *str)
  {
    return {str, lak::string_length(str)};
  }

  template<typename CHAR>
  lak::span<const CHAR> string_view(const std::basic_string<CHAR> &str)
  {
    auto begin = str.c_str();
    return {begin, begin + str.length()};
  }

  template<typename CHAR>
  uint8_t character_length(const std::basic_string<CHAR> &str, size_t offset)
  {
    return lak::character_length(lak::string_view(str).subspan(offset));
  }

  template<typename CHAR>
  uint8_t codepoint(const std::basic_string<CHAR> &str, size_t offset)
  {
    return lak::codepoint(lak::string_view(str).subspan(offset));
  }

  template<typename CHAR>
  void append_codepoint(std::basic_string<CHAR> &str, char32_t code)
  {
    constexpr size_t buffer_size = lak::chars_per_codepoint_v<CHAR>;
    CHAR buffer[buffer_size + 1] = {};
    lak::from_codepoint(lak::span(buffer).template first<buffer_size>(), code);
    str += buffer;
  }
}