#include "lak/structure/tiff.hpp"

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

template<typename T>
requires(
#define LAK_TIFF_IFD_TAG(VAL, NAME, TYPE, ...) lak::is_same_v<T, TYPE> ||
  LAK_FOREACH_TIFF_TYPE(LAK_TIFF_IFD_TAG)
#undef LAK_TIFF_IFD_TAG
    false)
void lak::tiff::ifd_tag::set_data(lak::array<T> &&new_data)
{
	_data_store.force_clear();
	lak::fill<byte_t>(this->_value, byte_t(0));
	lak::span<T> d;
	if (new_data.size() * sizeof(T) <= 4U)
	{
		lak::memcpy(lak::span(this->_value),
		            lak::span<const byte_t>(lak::span(new_data)));
		d = lak::span<T>(lak::span<byte_t>(this->_value)).first(new_data.size());
	}
	else
	{
		_data_store = new_data.release_as_bytes();
		d           = lak::span<T>(lak::span(_data_store));
	}
	data = d;
}

template<typename T>
requires(
#define LAK_TIFF_IFD_TAG(VAL, NAME, TYPE, ...) lak::is_same_v<T, TYPE> ||
  LAK_FOREACH_TIFF_TYPE(LAK_TIFF_IFD_TAG)
#undef LAK_TIFF_IFD_TAG
    false)
void lak::tiff::ifd_tag::set_data(lak::span<const T> new_data)
{
	_data_store.force_clear();
	lak::fill<byte_t>(this->_value, byte_t(0));
	lak::span<T> d;
	if (new_data.size() * sizeof(T) <= 4U)
	{
		lak::memcpy(lak::span(this->_value), lak::span<const byte_t>(new_data));
		d = lak::span<T>(lak::span<byte_t>(this->_value)).first(new_data.size());
	}
	else
	{
		_data_store.resize(new_data.size() * sizeof(T));
		lak::memcpy(lak::span(_data_store), lak::span<const byte_t>(new_data));
		d = lak::span<T>(lak::span(_data_store));
	}
	data = d;
}

template<lak::endian E>
lak::tiff::result<> lak::tiff::ifd_tag::read(lak::binary_reader &strm)
{
	RES_TRY_ASSIGN(id =, strm.template read<lak::tiff::tag_name, E>());
	RES_TRY_ASSIGN(auto type =, strm.template read<lak::tiff::tag_type, E>());
	RES_TRY_ASSIGN(uint32_t count =, strm.template read_u32<E>());

	switch (type)
	{
#define LAK_TIFF_TAG_READ(VAL, NAME, TYPE, ...)                               \
	case lak::tiff::tag_type::NAME:                                             \
	{                                                                           \
		if (count * sizeof(TYPE) <= 4U)                                           \
		{                                                                         \
			/* inline data */                                                       \
			size_t pos = strm.position();                                           \
			auto new_data =                                                         \
			  lak::span<TYPE>(lak::span<byte_t>(_value)).first(count);              \
			data = new_data;                                                        \
			for (auto &v : new_data)                                                \
			{                                                                       \
				RES_TRY_ASSIGN(v =, strm.template read<TYPE, E>());                   \
			}                                                                       \
			RES_TRY(strm.seek(pos + 4U));                                           \
		}                                                                         \
		else                                                                      \
		{                                                                         \
			/* external data */                                                     \
			size_t pos;                                                             \
			{                                                                       \
				RES_TRY_ASSIGN(uint32_t offset =, strm.template read_u32<E>());       \
				pos = strm.position();                                                \
				RES_TRY(strm.seek(offset));                                           \
			}                                                                       \
			RES_TRY_ASSIGN(auto new_data =, strm.template read<TYPE, E>(count));    \
			set_data(lak::move(new_data));                                          \
			RES_TRY(strm.seek(pos));                                                \
		}                                                                         \
	}                                                                           \
	break;
		LAK_FOREACH_TIFF_TYPE(LAK_TIFF_TAG_READ)
#undef LAK_TIFF_TAG_READ
		default:
			ASSERT_UNREACHABLE();
	}

	return lak::ok_t{};
}

size_t lak::tiff::ifd_tag::write_size() const
{
	// id + type + count + offset + external data size
	return 2U + 2U + 4U + 4U + lak::to_multiple<size_t>(_data_store.size(), 4U);
}

template<lak::endian E>
lak::tiff::result<> lak::tiff::ifd_tag::write(
  lak::binary_span_writer &strm, lak::binary_span_writer &ext_strm) const
{
	RES_TRY(strm.template write<E>(id));
#define LAK_TIFF_TAG_WRITE(VAL, NAME, TYPE, ...)                              \
	[&](lak::span<const TYPE> data) -> lak::tiff::result<>                      \
	{                                                                           \
		RES_TRY(strm.template write<E>(lak::tiff::tag_type::NAME));               \
		ASSERT_LESS(data.size(), UINT32_MAX);                                     \
		RES_TRY(strm.template write_u32<E>(static_cast<uint32_t>(data.size())));  \
		if (data.size() * sizeof(TYPE) <= 4U)                                     \
		{                                                                         \
			/* inline data */                                                       \
			RES_TRY(strm.template write<E>(data));                                  \
			for (size_t j = data.size() * sizeof(TYPE); j < 4U; ++j)                \
			{                                                                       \
				RES_TRY(strm.template write_u8<E>(0U));                               \
			}                                                                       \
		}                                                                         \
		else                                                                      \
		{                                                                         \
			/* external data */                                                     \
			ASSERT_LESS(ext_strm.position(), UINT32_MAX);                           \
			RES_TRY(strm.template write_u32<E>(                                     \
			  static_cast<uint32_t>(ext_strm.position())));                         \
			RES_TRY(ext_strm.template write<E>(data));                              \
			RES_TRY(                                                                \
			  ext_strm.seek(lak::to_multiple<size_t>(ext_strm.position(), 4U))      \
			    .replace_err(lak::out_of_data_error{}));                            \
		}                                                                         \
		return lak::ok_t{};                                                       \
	},
	RES_TRY(visit(lak::overloaded{LAK_FOREACH_TIFF_TYPE(LAK_TIFF_TAG_WRITE)}));
#undef LAK_TIFF_TAG_WRITE
	return lak::ok_t{};
}

/* --- image_file_directory --- */

template<lak::endian E>
lak::tiff::result<> lak::tiff::image_file_directory::read(
  lak::binary_reader &strm)
{
	RES_TRY_ASSIGN(uint16_t tag_count =, strm.template read_u16<E>());
	tags.resize(tag_count);

	lak::optional<lak::tiff::ifd_tag &> strip_offsets;
	lak::optional<lak::tiff::ifd_tag &> rows_per_strip;
	lak::optional<lak::tiff::ifd_tag &> strip_byte_counts;
	lak::optional<lak::tiff::ifd_tag &> subifd_tag;

	size_t ti = 0;
	for (auto &t : tags)
	{
		++ti;
		RES_TRY(t.template read<E>(strm));

		switch (t.id)
		{
			case lak::tiff::tag_name::StripOffsets:
				strip_offsets = t;
				break;
			case lak::tiff::tag_name::RowsPerStrip:
				rows_per_strip = t;
				break;
			case lak::tiff::tag_name::StripByteCounts:
				strip_byte_counts = t;
				break;
			case lak::tiff::tag_name::SubIFDs:
				subifd_tag = t;
				break;
			default:
				break;
		}
	}

	RES_TRY_ASSIGN(_ifd_offset =, strm.template read_u32<E>());

	size_t pos = strm.position();
	if (strip_offsets && strip_byte_counts && rows_per_strip)
	{
		strip_byte_counts->visit(lak::overloaded{
		  [&]<lak::concepts::one_of<uint16_t, uint32_t> T>(
		    const lak::span<T> &sizes_data)
		  {
			  strips.reserve(sizes_data.size());
			  for (auto s : sizes_data) strips.emplace_back().data.resize(s);
		  },
		  [](auto &&a) { ASSERTF_UNREACHABLE(typeid(a).name()); },
		});

		rows_per_strip->visit(lak::overloaded{
		  [&]<lak::concepts::one_of<uint16_t, uint32_t> T>(
		    const lak::span<T> &rows_data)
		  {
			  ASSERT_EQUAL(rows_data.size(), 1U);
			  rows = rows_data[0];
		  },
		  [](auto &&a) { ASSERTF_UNREACHABLE(typeid(a).name()); },
		});

		RES_TRY(strip_offsets->visit(lak::overloaded{
		  [&]<lak::concepts::one_of<uint16_t, uint32_t> T>(
		    const lak::span<T> &off_data) -> lak::tiff::result<>
		  {
			  if (off_data.size() > strips.size()) strips.resize(off_data.size());
			  for (size_t o = 0U; o < off_data.size(); ++o)
			  {
				  RES_TRY(strm.seek(off_data[o]));
				  RES_TRY_ASSIGN(auto bytes =, strm.read_bytes(strips[o].data.size()));
				  lak::memcpy(strips[o].data, bytes);
			  }
			  return lak::ok_t{};
		  },
		  [](auto &&a) -> lak::tiff::result<>
		  { ASSERTF_UNREACHABLE(typeid(a).name()); },
		}));
	}

	if (subifd_tag)
	{
		RES_TRY(subifd_tag->data.visit(lak::overloaded{
		  [&](lak::span<uint32_t> offsets) -> lak::tiff::result<>
		  {
			  const size_t pos = strm.position();
			  subifds.reserve(offsets.size());
			  for (auto &off : offsets)
			  {
				  size_t _off = off;
				  while (_off != 0U)
				  {
					  RES_TRY(strm.seek(_off));
					  auto &subifd = subifds.emplace_back();
					  RES_TRY(subifd.template read<E>(strm));
					  _off = subifd._ifd_offset;
				  }
			  }
			  RES_TRY(strm.seek(pos));
			  return lak::ok_t{};
		  },
		  [](auto &&) -> lak::tiff::result<> { ASSERT_UNREACHABLE(); }}));
	}

	RES_TRY(strm.seek(pos));

	return lak::ok_t{};
}

size_t lak::tiff::image_file_directory::write_size() const
{
	size_t result = 2U + 4U; // tag count + next ifd offset
	if (!strips.empty())
	{
		result += 3U * 12U;
		if (strips.size() > 1U) result += 2U * sizeof(char32_t) * strips.size();
		for (const auto &s : strips) result += s.data.size();
	}
	for (const auto &t : tags) result += t.write_size();
	return result;
}

template<lak::endian E>
lak::tiff::result<> lak::tiff::image_file_directory::write(
  lak::binary_span_writer &strm, lak::binary_span_writer &ext_strm) const
{
	RES_TRY(ext_strm.skip((12U * tags.size()) + 4U)
	          .replace_err(lak::out_of_data_error{}));

	lak::optional<lak::tiff::ifd_tag> strip_offsets;
	lak::optional<lak::tiff::ifd_tag> rows_per_strip;
	lak::optional<lak::tiff::ifd_tag> strip_byte_counts;

	if (!strips.empty())
	{
		lak::array<uint32_t> offsets;
		offsets.reserve(strips.size());
		lak::array<uint32_t> counts;
		counts.reserve(strips.size());

		for (const auto &s : strips)
		{
			offsets.push_back(static_cast<uint32_t>(ext_strm.position()));
			RES_TRY(ext_strm.template write<E>(lak::span(s.data)));
			counts.push_back(static_cast<uint32_t>(s.data.size()));
		}

		strip_offsets = lak::tiff::ifd_tag::make_StripOffsets(lak::move(offsets));
		rows_per_strip =
		  lak::tiff::ifd_tag::make_RowsPerStrip(lak::fixed_array(rows));
		strip_byte_counts =
		  lak::tiff::ifd_tag::make_StripByteCounts(lak::move(counts));
	}

	const size_t tag_count = tags.size() + (strips.empty() ? 0U : 3U);
	ASSERT_LESS(tag_count, UINT16_MAX);
	RES_TRY(strm.template write_u16<E>(static_cast<uint16_t>(tag_count)));

	size_t ti = 0;
	for (const auto &t : tags)
	{
		if (strip_offsets &&
		    static_cast<uint16_t>(t.id) >
		      static_cast<uint16_t>(lak::tiff::tag_name::StripOffsets))
		{
			RES_TRY(strip_offsets->template write<E>(strm, ext_strm));
			strip_offsets.reset();
			++ti;
		}
		if (rows_per_strip &&
		    static_cast<uint16_t>(t.id) >
		      static_cast<uint16_t>(lak::tiff::tag_name::RowsPerStrip))
		{
			RES_TRY(rows_per_strip->template write<E>(strm, ext_strm));
			rows_per_strip.reset();
			++ti;
		}
		if (strip_byte_counts &&
		    static_cast<uint16_t>(t.id) >
		      static_cast<uint16_t>(lak::tiff::tag_name::StripByteCounts))
		{
			RES_TRY(strip_byte_counts->template write<E>(strm, ext_strm));
			strip_byte_counts.reset();
			++ti;
		}

		++ti;
		RES_TRY(t.template write<E>(strm, ext_strm));
	}

	if (strip_offsets)
	{
		RES_TRY(strip_offsets->template write<E>(strm, ext_strm));
		strip_offsets.reset();
	}
	if (rows_per_strip)
	{
		RES_TRY(rows_per_strip->template write<E>(strm, ext_strm));
		rows_per_strip.reset();
	}
	if (strip_byte_counts)
	{
		RES_TRY(strip_byte_counts->template write<E>(strm, ext_strm));
		strip_byte_counts.reset();
	}
	return lak::ok_t{};
}

/* --- tiff --- */

template<lak::endian E>
lak::tiff::result<> lak::tiff::tiff::_read(lak::binary_reader &strm)
{
	ifd.clear();

	RES_TRY_ASSIGN(ifh.version =, strm.template read_u16<E>());
	RES_TRY_ASSIGN(uint32_t ifd_offset =, strm.template read_u32<E>());
	while (ifd_offset != 0U)
	{
		RES_TRY(strm.seek(ifd_offset));

		auto &i = ifd.emplace_back();
		RES_TRY(i.template read<E>(strm));

		ifd_offset = i._ifd_offset;
	}
	return lak::ok_t{};
}

template<lak::endian E>
lak::tiff::result<> lak::tiff::tiff::read(lak::binary_reader &strm)
{
	// reading tiffs requires reading at exact offsets from the start of the
	// file, thus we must ensure our stream is starting at position 0.
	lak::binary_reader rstrm{strm.remaining()};

	auto update_strm = [&](auto)
	{
		strm.skip(rstrm.position()).UNWRAP();
		return lak::monostate{};
	};

	RES_TRY_ASSIGN(uint16_t byte_order =, strm.read_u16le());
	if (byte_order == u8"II"_magic_ne)
		return _read<lak::endian::little>(strm).map(update_strm);
	else if (byte_order == u8"MM"_magic_ne)
		return _read<lak::endian::big>(strm).map(update_strm);
	else
		return lak::err_t<lak::value_out_of_range_error>{};
}

template<lak::endian E>
size_t lak::tiff::tiff::write_size() const
{
	size_t result = 8U; // ifh
	for (const auto &d : ifd) result += d.write_size();
	return result;
}

template<lak::endian E>
requires(E == lak::endian::little || E == lak::endian::big)
lak::tiff::result<> lak::tiff::tiff::write(lak::binary_span_writer &strm) const
{
	lak::binary_span_writer ext_strm{strm._data};

	if constexpr (E == lak::endian::little)
	{
		RES_TRY(strm.write_u16le(u8"II"_magic_ne));
	}
	else
	{
		RES_TRY(strm.write_u16le(u8"MM"_magic_ne));
	}
	RES_TRY(strm.template write_u16<E>(ifh.version));

	RES_TRY(ext_strm.seek((strm.position()) + 4U)
	          .replace_err(lak::out_of_data_error{}));

	for (const auto &i : ifd)
	{
		const size_t next = ext_strm.position();
		ASSERT_LESS(next, UINT32_MAX);
		RES_TRY(strm.template write_u32<E>(static_cast<uint32_t>(next)));
		RES_TRY(strm.seek(next).replace_err(lak::out_of_data_error{}));
		RES_TRY(ext_strm.seek(next).replace_err(lak::out_of_data_error{}));

		RES_TRY(i.template write<E>(strm, ext_strm));
	}

	RES_TRY(strm.template write_u32<E>(0U));

	return lak::ok_t{};
}
