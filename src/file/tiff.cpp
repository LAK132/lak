#include "lak/file/tiff.hpp"

#include "lak/string_literals/magic.hpp"

/* --- ifd_tag --- */

lak::tiff::ifd_tag::ifd_tag(ifd_tag &&other)
: id(other.id),
  _data_store(lak::exchange(other._data_store, lak::array<byte_t>{}))
{
	other.data.visit(
	  [&]<typename T>(lak::span<T> d)
	  {
		  if (!_data_store.empty())
			  data = lak::span<T>(lak::span(_data_store));
		  else
		  {
			  lak::memcpy(_value, other._value);
			  data = lak::span<T>(lak::span<byte_t>(_value)).first(d.size());
		  }
	  });
	other.data = lak::span<byte_t>{};
}

lak::tiff::ifd_tag &lak::tiff::ifd_tag::operator=(ifd_tag &&other)
{
	id          = other.id;
	_data_store = lak::exchange(other._data_store, lak::array<byte_t>{});
	other.data.visit(
	  [&]<typename T>(lak::span<T> d)
	  {
		  if (!_data_store.empty())
			  data = lak::span<T>(lak::span(_data_store));
		  else
		  {
			  lak::memcpy(_value, other._value);
			  data = lak::span<T>(lak::span<byte_t>(_value)).first(d.size());
		  }
	  });
	other.data = lak::span<byte_t>{};
	return *this;
}

#define LAK_TIFF_TAG_MAKE(NAME, TYPE, COUNT, ...)                             \
	lak::tiff::ifd_tag lak::tiff::ifd_tag::make_##NAME(                         \
	  lak::span<const TYPE, COUNT> data)                                        \
	{                                                                           \
		ifd_tag result{};                                                         \
		result.id = lak::tiff::tag_name::NAME;                                    \
		result.template set_data<TYPE>(data);                                     \
		return result;                                                            \
	}                                                                           \
	lak::tiff::ifd_tag lak::tiff::ifd_tag::make_##NAME(                         \
	  lak::array<TYPE, COUNT> &&data)                                           \
	{                                                                           \
		ifd_tag result{};                                                         \
		result.id = lak::tiff::tag_name::NAME;                                    \
		result.template set_data<TYPE>(lak::move(data));                          \
		return result;                                                            \
	}
LAK_FOREACH_TIFF_TAG_TYPES(LAK_TIFF_TAG_MAKE)
#undef LAK_TIFF_TAG_MAKE

/* --- image_file_directory --- */

size_t lak::tiff::image_file_directory::write_size() const
{
	size_t result = _write_size();

	size_t ext_tag_data = 0U;
	for (const auto &t : tags) ext_tag_data += t._data_write_size();
	result += ext_tag_data;

	if (!strips.empty() || !subifds.empty() || !!exif || !!ext_tag_data)
		result = lak::to_multiple<size_t>(result, 4U);

	if (strips.size() > 1U) result += 2U * (4U * strips.size());
	for (const auto &s : strips)
		result += lak::to_multiple<size_t>(s.data.size(), 4U);
	for (const auto &ifd : subifds)
		result += lak::to_multiple<size_t>(ifd.write_size(), 4U);
	if (exif) result += lak::to_multiple<size_t>(exif->write_size(), 4U);

	return result;
}

#define LAK_TIFF_TAG_MAKE(NAME, TYPE, COUNT, ...)                             \
	void lak::tiff::image_file_directory::push_##NAME(                          \
	  lak::span<const TYPE, COUNT> data)                                        \
	{                                                                           \
		push<TYPE>(lak::tiff::tag_name::NAME, data);                              \
	}
LAK_FOREACH_TIFF_TAG_TYPES(LAK_TIFF_TAG_MAKE)
#undef LAK_TIFF_TAG_MAKE

#define LAK_TIFF_TAG_MAKE(NAME, ...)                                          \
	void lak::tiff::image_file_directory::push_##NAME(lak::astring_view data)   \
	{                                                                           \
		push(lak::tiff::tag_name::NAME, data);                                    \
	}
LAK_FOREACH_STRING_TIFF_TAG(LAK_TIFF_TAG_MAKE)
#undef LAK_TIFF_TAG_MAKE
