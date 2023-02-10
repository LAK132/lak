#include "lak/string_view.hpp"

inline lak::astring_view lak::as_astring(lak::astring_view str) { return str; }

inline lak::astring_view lak::as_astring(lak::u8string_view str)
{
	return lak::astring_view(reinterpret_cast<const char *>(str.data()),
	                         str.size());
}

inline lak::u8string_view lak::as_u8string(lak::astring_view str)
{
	return lak::u8string_view(reinterpret_cast<const char8_t *>(str.data()),
	                          str.size());
}

inline lak::u8string_view lak::as_u8string(lak::u8string_view str)
{
	return str;
}

inline const char *lak::as_astring(const char *str) { return str; }

inline const char *lak::as_astring(const char8_t *str)
{
	return reinterpret_cast<const char *>(str);
}

inline const char8_t *lak::as_u8string(const char *str)
{
	return reinterpret_cast<const char8_t *>(str);
}

inline const char8_t *lak::as_u8string(const char8_t *str) { return str; }
