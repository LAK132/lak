namespace lak
{
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
    CHAR c[lak::chars_per_codepoint_v<CHAR> + 1] = {};
    lak::from_codepoint(lak::span(c).first(lak::chars_per_codepoint_v<CHAR>),
                        code);
    str += c;
  }
}