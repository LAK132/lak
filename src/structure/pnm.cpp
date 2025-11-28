#include "lak/structure/pnm.hpp"

#include "lak/bit_reader.hpp"
#include "lak/overloaded.hpp"

#include "lak/dsl/binary_reader.hpp"
#include "lak/dsl/dsl.hpp"
#include "lak/dsl/utility.hpp"

uint8_t lak::pnm::pnm::channels() const
{
	return value.visit(::lak::overloaded{
	  [](const ::lak::monostate &) -> uint8_t { return 0; },
	  [](const pnm_data<bool, bool> &) -> uint8_t { return 1; },
	  [](const pnm_data<uint8_t, uint8_t> &) -> uint8_t { return 1; },
	  [](const pnm_data<uint16_t, uint16_t> &) -> uint8_t { return 1; },
	  [](const pnm_data<f32_t, f32_t> &) -> uint8_t { return 1; },
	  [](const pnm_data<::lak::vec3u8_t, uint8_t> &) -> uint8_t { return 3; },
	  [](const pnm_data<::lak::vec4u8_t, uint8_t> &) -> uint8_t { return 4; },
	  [](const pnm_data<::lak::vec3u16_t, uint16_t> &) -> uint8_t { return 3; },
	  [](const pnm_data<::lak::vec4u16_t, uint16_t> &) -> uint8_t { return 4; },
	  [](const pnm_data<::lak::vec3f32_t, f32_t> &) -> uint8_t { return 3; },
	  [](const pnm_data<::lak::vec4f32_t, f32_t> &) -> uint8_t { return 4; },
	});
}

lak::pnm::pnm::operator ::lak::image3_t() const
{
	return value.visit(::lak::overloaded{
	  [](const ::lak::monostate &) -> ::lak::image3_t { ASSERT_UNREACHABLE(); },
	  [this](const pnm_data<bool, bool> &data) -> ::lak::image3_t
	  {
		  ::lak::image3_t result;
		  result.resize({width, data.size() / width});
		  size_t i = 0U;
		  for (const auto &v : data)
		  {
			  result[i].r = v ? 0U : UINT8_MAX;
			  result[i].g = v ? 0U : UINT8_MAX;
			  result[i].b = v ? 0U : UINT8_MAX;
			  ++i;
		  }
		  return result;
	  },
	  [this](const pnm_data<uint8_t, uint8_t> &data) -> ::lak::image3_t
	  {
		  ::lak::image3_t result;
		  result.resize({width, data.size() / width});
		  size_t i = 0U;
		  if (data.max_value == UINT8_MAX)
			  for (const auto &v : data)
			  {
				  result[i].r = v;
				  result[i].g = v;
				  result[i].b = v;
				  ++i;
			  }
		  else
			  for (const auto &v : data)
			  {
				  result[i].r = uint8_t((uintmax_t(v) * UINT8_MAX) / data.max_value);
				  result[i].g = uint8_t((uintmax_t(v) * UINT8_MAX) / data.max_value);
				  result[i].b = uint8_t((uintmax_t(v) * UINT8_MAX) / data.max_value);
				  ++i;
			  }
		  return result;
	  },
	  [this](const pnm_data<uint16_t, uint16_t> &data) -> ::lak::image3_t
	  {
		  ::lak::image3_t result;
		  result.resize({width, data.size() / width});
		  size_t i = 0U;
		  for (const auto &v : data)
		  {
			  result[i].r = uint8_t((uintmax_t(v) * UINT8_MAX) / data.max_value);
			  result[i].g = uint8_t((uintmax_t(v) * UINT8_MAX) / data.max_value);
			  result[i].b = uint8_t((uintmax_t(v) * UINT8_MAX) / data.max_value);
			  ++i;
		  }
		  return result;
	  },
	  [this](const pnm_data<f32_t, f32_t> &data) -> ::lak::image3_t
	  {
		  ::lak::image3_t result;
		  result.resize({width, data.size() / width});
		  size_t i = 0U;
		  for (const auto &v : data)
		  {
			  result[i].r = uint8_t((v * UINT8_MAX) / data.max_value);
			  result[i].g = uint8_t((v * UINT8_MAX) / data.max_value);
			  result[i].b = uint8_t((v * UINT8_MAX) / data.max_value);
			  ++i;
		  }
		  return result;
	  },
	  [this](const pnm_data<::lak::vec3u8_t, uint8_t> &data) -> ::lak::image3_t
	  {
		  ::lak::image3_t result;
		  result.resize({width, data.size() / width});
		  size_t i = 0U;
		  if (data.max_value == UINT8_MAX)
			  for (const auto &v : data) result[i++] = v;
		  else
			  for (const auto &v : data)
			  {
				  result[i].r = uint8_t((uintmax_t(v.r) * UINT8_MAX) / data.max_value);
				  result[i].g = uint8_t((uintmax_t(v.g) * UINT8_MAX) / data.max_value);
				  result[i].b = uint8_t((uintmax_t(v.b) * UINT8_MAX) / data.max_value);
				  ++i;
			  }
		  return result;
	  },
	  [this](const pnm_data<::lak::vec4u8_t, uint8_t> &data) -> ::lak::image3_t
	  {
		  ::lak::image3_t result;
		  result.resize({width, data.size() / width});
		  size_t i = 0U;
		  if (data.max_value == UINT8_MAX)
			  for (const auto &v : data)
			  {
				  result[i].r = v.r;
				  result[i].g = v.g;
				  result[i].b = v.b;
				  ++i;
			  }
		  else
			  for (const auto &v : data)
			  {
				  result[i].r = uint8_t((uintmax_t(v.r) * UINT8_MAX) / data.max_value);
				  result[i].g = uint8_t((uintmax_t(v.g) * UINT8_MAX) / data.max_value);
				  result[i].b = uint8_t((uintmax_t(v.b) * UINT8_MAX) / data.max_value);
				  ++i;
			  }
		  return result;
	  },
	  [this](const pnm_data<::lak::vec3u16_t, uint16_t> &data) -> ::lak::image3_t
	  {
		  ::lak::image3_t result;
		  result.resize({width, data.size() / width});
		  size_t i = 0U;
		  for (const auto &v : data)
		  {
			  result[i].r = uint8_t((uintmax_t(v.r) * UINT8_MAX) / data.max_value);
			  result[i].g = uint8_t((uintmax_t(v.g) * UINT8_MAX) / data.max_value);
			  result[i].b = uint8_t((uintmax_t(v.b) * UINT8_MAX) / data.max_value);
			  ++i;
		  }
		  return result;
	  },
	  [this](const pnm_data<::lak::vec4u16_t, uint16_t> &data) -> ::lak::image3_t
	  {
		  ::lak::image3_t result;
		  result.resize({width, data.size() / width});
		  size_t i = 0U;
		  for (const auto &v : data)
		  {
			  result[i].r = uint8_t((uintmax_t(v.r) * UINT8_MAX) / data.max_value);
			  result[i].g = uint8_t((uintmax_t(v.g) * UINT8_MAX) / data.max_value);
			  result[i].b = uint8_t((uintmax_t(v.b) * UINT8_MAX) / data.max_value);
			  ++i;
		  }
		  return result;
	  },
	  [this](const pnm_data<::lak::vec3f32_t, f32_t> &data) -> ::lak::image3_t
	  {
		  ::lak::image3_t result;
		  result.resize({width, data.size() / width});
		  size_t i = 0U;
		  for (const auto &v : data)
		  {
			  result[i].r = uint8_t((v.r * UINT8_MAX) / data.max_value);
			  result[i].g = uint8_t((v.g * UINT8_MAX) / data.max_value);
			  result[i].b = uint8_t((v.b * UINT8_MAX) / data.max_value);
			  ++i;
		  }
		  return result;
	  },
	  [this](const pnm_data<::lak::vec4f32_t, f32_t> &data) -> ::lak::image3_t
	  {
		  ::lak::image3_t result;
		  result.resize({width, data.size() / width});
		  size_t i = 0U;
		  for (const auto &v : data)
		  {
			  result[i].r = uint8_t((v.r * UINT8_MAX) / data.max_value);
			  result[i].g = uint8_t((v.g * UINT8_MAX) / data.max_value);
			  result[i].b = uint8_t((v.b * UINT8_MAX) / data.max_value);
			  ++i;
		  }
		  return result;
	  },
	});
}

lak::pnm::pnm::operator ::lak::image4_t() const
{
	return value.visit(::lak::overloaded{
	  [](const ::lak::monostate &) -> ::lak::image4_t { ASSERT_UNREACHABLE(); },
	  [this](const pnm_data<bool, bool> &data) -> ::lak::image4_t
	  {
		  ::lak::image4_t result;
		  result.resize({width, data.size() / width});
		  size_t i = 0U;
		  for (const auto &v : data)
		  {
			  result[i].r = v ? 0U : UINT8_MAX;
			  result[i].g = v ? 0U : UINT8_MAX;
			  result[i].b = v ? 0U : UINT8_MAX;
			  result[i].a = UINT8_MAX;
			  ++i;
		  }
		  return result;
	  },
	  [this](const pnm_data<uint8_t, uint8_t> &data) -> ::lak::image4_t
	  {
		  ::lak::image4_t result;
		  result.resize({width, data.size() / width});
		  size_t i = 0U;
		  if (data.max_value == UINT8_MAX)
			  for (const auto &v : data)
			  {
				  result[i].r = v;
				  result[i].g = v;
				  result[i].b = v;
				  result[i].a = UINT8_MAX;
				  ++i;
			  }
		  else
			  for (const auto &v : data)
			  {
				  result[i].r = uint8_t((uintmax_t(v) * UINT8_MAX) / data.max_value);
				  result[i].g = uint8_t((uintmax_t(v) * UINT8_MAX) / data.max_value);
				  result[i].b = uint8_t((uintmax_t(v) * UINT8_MAX) / data.max_value);
				  result[i].a = UINT8_MAX;
				  ++i;
			  }
		  return result;
	  },
	  [this](const pnm_data<uint16_t, uint16_t> &data) -> ::lak::image4_t
	  {
		  ::lak::image4_t result;
		  result.resize({width, data.size() / width});
		  size_t i = 0U;
		  for (const auto &v : data)
		  {
			  result[i].r = uint8_t((uintmax_t(v) * UINT8_MAX) / data.max_value);
			  result[i].g = uint8_t((uintmax_t(v) * UINT8_MAX) / data.max_value);
			  result[i].b = uint8_t((uintmax_t(v) * UINT8_MAX) / data.max_value);
			  result[i].a = UINT8_MAX;
			  ++i;
		  }
		  return result;
	  },
	  [this](const pnm_data<f32_t, f32_t> &data) -> ::lak::image4_t
	  {
		  ::lak::image4_t result;
		  result.resize({width, data.size() / width});
		  size_t i = 0U;
		  for (const auto &v : data)
		  {
			  result[i].r = uint8_t((v * UINT8_MAX) / data.max_value);
			  result[i].g = uint8_t((v * UINT8_MAX) / data.max_value);
			  result[i].b = uint8_t((v * UINT8_MAX) / data.max_value);
			  result[i].a = UINT8_MAX;
			  ++i;
		  }
		  return result;
	  },
	  [this](const pnm_data<::lak::vec3u8_t, uint8_t> &data) -> ::lak::image4_t
	  {
		  ::lak::image4_t result;
		  result.resize({width, data.size() / width});
		  size_t i = 0U;
		  if (data.max_value == UINT8_MAX)
			  for (const auto &v : data)
			  {
				  result[i].r = v.r;
				  result[i].g = v.g;
				  result[i].b = v.b;
				  result[i].a = UINT8_MAX;
				  ++i;
			  }
		  else
			  for (const auto &v : data)
			  {
				  result[i].r = uint8_t((uintmax_t(v.r) * UINT8_MAX) / data.max_value);
				  result[i].g = uint8_t((uintmax_t(v.g) * UINT8_MAX) / data.max_value);
				  result[i].b = uint8_t((uintmax_t(v.b) * UINT8_MAX) / data.max_value);
				  result[i].a = UINT8_MAX;
				  ++i;
			  }
		  return result;
	  },
	  [this](const pnm_data<::lak::vec4u8_t, uint8_t> &data) -> ::lak::image4_t
	  {
		  ::lak::image4_t result;
		  result.resize({width, data.size() / width});
		  size_t i = 0U;
		  if (data.max_value == UINT8_MAX)
			  for (const auto &v : data) result[i++] = v;
		  else
			  for (const auto &v : data)
			  {
				  result[i].r = uint8_t((uintmax_t(v.r) * UINT8_MAX) / data.max_value);
				  result[i].g = uint8_t((uintmax_t(v.g) * UINT8_MAX) / data.max_value);
				  result[i].b = uint8_t((uintmax_t(v.b) * UINT8_MAX) / data.max_value);
				  result[i].a = uint8_t((uintmax_t(v.a) * UINT8_MAX) / data.max_value);
				  ++i;
			  }
		  return result;
	  },
	  [this](const pnm_data<::lak::vec3u16_t, uint16_t> &data) -> ::lak::image4_t
	  {
		  ::lak::image4_t result;
		  result.resize({width, data.size() / width});
		  size_t i = 0U;
		  for (const auto &v : data)
		  {
			  result[i].r = uint8_t((uintmax_t(v.r) * UINT8_MAX) / data.max_value);
			  result[i].g = uint8_t((uintmax_t(v.g) * UINT8_MAX) / data.max_value);
			  result[i].b = uint8_t((uintmax_t(v.b) * UINT8_MAX) / data.max_value);
			  result[i].a = UINT8_MAX;
			  ++i;
		  }
		  return result;
	  },
	  [this](const pnm_data<::lak::vec4u16_t, uint16_t> &data) -> ::lak::image4_t
	  {
		  ::lak::image4_t result;
		  result.resize({width, data.size() / width});
		  size_t i = 0U;
		  for (const auto &v : data)
		  {
			  result[i].r = uint8_t((uintmax_t(v.r) * UINT8_MAX) / data.max_value);
			  result[i].g = uint8_t((uintmax_t(v.g) * UINT8_MAX) / data.max_value);
			  result[i].b = uint8_t((uintmax_t(v.b) * UINT8_MAX) / data.max_value);
			  result[i].a = uint8_t((uintmax_t(v.a) * UINT8_MAX) / data.max_value);
			  ++i;
		  }
		  return result;
	  },
	  [this](const pnm_data<::lak::vec3f32_t, f32_t> &data) -> ::lak::image4_t
	  {
		  ::lak::image4_t result;
		  result.resize({width, data.size() / width});
		  size_t i = 0U;
		  for (const auto &v : data)
		  {
			  result[i].r = uint8_t((v.r * UINT8_MAX) / data.max_value);
			  result[i].g = uint8_t((v.g * UINT8_MAX) / data.max_value);
			  result[i].b = uint8_t((v.b * UINT8_MAX) / data.max_value);
			  result[i].a = UINT8_MAX;
			  ++i;
		  }
		  return result;
	  },
	  [this](const pnm_data<::lak::vec4f32_t, f32_t> &data) -> ::lak::image4_t
	  {
		  ::lak::image4_t result;
		  result.resize({width, data.size() / width});
		  size_t i = 0U;
		  for (const auto &v : data)
		  {
			  result[i].r = uint8_t((v.r * UINT8_MAX) / data.max_value);
			  result[i].g = uint8_t((v.g * UINT8_MAX) / data.max_value);
			  result[i].b = uint8_t((v.b * UINT8_MAX) / data.max_value);
			  result[i].a = uint8_t((v.a * UINT8_MAX) / data.max_value);
			  ++i;
		  }
		  return result;
	  },
	});
}

static constexpr auto type_parser =
  lak::dsl::replace_str_literal<u8"P1", lak::pnm::pnm_type::P1> |
  lak::dsl::replace_str_literal<u8"P2", lak::pnm::pnm_type::P2> |
  lak::dsl::replace_str_literal<u8"P3", lak::pnm::pnm_type::P3> |
  lak::dsl::replace_str_literal<u8"P4", lak::pnm::pnm_type::P4> |
  lak::dsl::replace_str_literal<u8"P5", lak::pnm::pnm_type::P5> |
  lak::dsl::replace_str_literal<u8"P6", lak::pnm::pnm_type::P6> |
  lak::dsl::replace_str_literal<u8"P7", lak::pnm::pnm_type::P7> |
  lak::dsl::replace_str_literal<u8"PF4", lak::pnm::pnm_type::PF4> |
  lak::dsl::replace_str_literal<u8"PF", lak::pnm::pnm_type::PF> |
  lak::dsl::replace_str_literal<u8"Pf", lak::pnm::pnm_type::Pf>;

static constexpr auto non_breaking_whitespace =
  (!lak::dsl::char_literal<U'\n'>)&lak::dsl::whitespace;

enum struct tupltype_t
{
	INVALID,
	BLACKANDWHITE,
	GRAYSCALE,
	RGB,
	BLACKANDWHITE_ALPHA,
	GRAYSCALE_ALPHA,
	RGB_ALPHA,
};

static constexpr auto tupltype_parser = lak::dsl::capture_nth<
  1U,
  lak::dsl::str_literal<u8"TUPLTYPE"> + (+non_breaking_whitespace),
  (lak::dsl::replace_str_literal<u8"BLACKANDWHITE",
                                 tupltype_t::BLACKANDWHITE> |
   lak::dsl::replace_str_literal<u8"GRAYSCALE", tupltype_t::GRAYSCALE> |
   lak::dsl::replace_str_literal<u8"RGB", tupltype_t::RGB> |
   lak::dsl::replace_str_literal<u8"BLACKANDWHITE_ALPHA",
                                 tupltype_t::BLACKANDWHITE_ALPHA> |
   lak::dsl::replace_str_literal<u8"GRAYSCALE_ALPHA",
                                 tupltype_t::GRAYSCALE_ALPHA> |
   lak::dsl::replace_str_literal<u8"RGB_ALPHA", tupltype_t::RGB_ALPHA>),
  lak::dsl::until_inc_char<U'\n'> +
    (*(lak::dsl::pound_line_comment + lak::dsl::whitespace))>;

static constexpr auto width_parser = lak::dsl::capture_nth<
  1U,
  lak::dsl::str_literal<u8"WIDTH"> + (+non_breaking_whitespace),
  lak::dsl::parsed_dec_uint<uint32_t>,
  lak::dsl::until_inc_char<U'\n'> +
    (*(lak::dsl::pound_line_comment + lak::dsl::whitespace))>;

static constexpr auto height_parser = lak::dsl::capture_nth<
  1U,
  lak::dsl::str_literal<u8"HEIGHT"> + (+non_breaking_whitespace),
  lak::dsl::parsed_dec_uint<uint32_t>,
  lak::dsl::until_inc_char<U'\n'> +
    (*(lak::dsl::pound_line_comment + lak::dsl::whitespace))>;

static constexpr auto depth_parser = lak::dsl::capture_nth<
  1U,
  lak::dsl::str_literal<u8"DEPTH"> + (+non_breaking_whitespace),
  lak::dsl::parsed_dec_uint<uint8_t>,
  lak::dsl::until_inc_char<U'\n'> +
    (*(lak::dsl::pound_line_comment + lak::dsl::whitespace))>;

static constexpr auto maxval_parser = lak::dsl::capture_nth<
  1U,
  lak::dsl::str_literal<u8"MAXVAL"> + (+non_breaking_whitespace),
  lak::dsl::parsed_dec_uint<uint16_t>,
  lak::dsl::until_inc_char<U'\n'> +
    (*(lak::dsl::pound_line_comment + lak::dsl::whitespace))>;

static constexpr auto endhdr_parser =
  lak::dsl::str_literal<u8"ENDHDR"> + lak::dsl::until_inc_char<U'\n'>;

static constexpr auto header_parser = lak::dsl::capture_nth<
  0U,
  lak::dsl::unordered<width_parser,
                      height_parser,
                      depth_parser,
                      maxval_parser,
                      lak::dsl::optional<tupltype_parser>>,
  endhdr_parser>;

::lak::error_codes<lak::err::out_of_data,
                   lak::err::value_out_of_range,
                   lak::err::string_to_numeric,
                   lak::dsl::err::parse>
lak::pnm::pnm::_read(::lak::binary_reader &strm)
{
	lak::dsl::binary_reader parser{strm};

	pnm_type type;
	uint32_t height    = 0U;
	bool ascii_encoded = false;

	[[maybe_unused]] auto value_check = []<typename T>(uintmax_t value,
	                                                   T max_value)
	  -> lak::result<T,
	                 lak::variant<lak::err::out_of_data,
	                              lak::err::value_out_of_range,
	                              lak::err::string_to_numeric,
	                              lak::dsl::err::parse>>
	{
		if (value > max_value)
			return lak::err_t{::lak::err::value_out_of_range{}};
		else
			return lak::ok_t{static_cast<T>(value)};
	};

	RES_TRY_ASSIGN(type =, parser.read(type_parser));

	RES_TRY(parser.read(*(lak::dsl::whitespace | lak::dsl::pound_line_comment)));

	if (type == pnm_type::P7)
	{
		[[maybe_unused]] tupltype_t tupltype = tupltype_t::INVALID;

		RES_TRY_ASSIGN(auto header =, parser.read(header_parser));

		width = header.template get<0>();

		height = header.template get<1>();

		uint8_t depth = header.template get<2>();
		if (depth > 4U) return lak::err_t{lak::err::value_out_of_range{}};

		uint16_t max_value = header.template get<3>();

		if (header.template get<4>().has_value())
			tupltype = *header.template get<4>();

		switch (depth)
		{
			case 1U:
			{
				if (max_value <= UINT8_MAX)
				{
					pnm_data<uint8_t, uint8_t> data;
					data.max_value = uint8_t(max_value);
					value          = ::lak::move(data);
				}
				else
				{
					pnm_data<uint16_t, uint16_t> data;
					data.max_value = uint16_t(max_value);
					value          = ::lak::move(data);
				}
			}
			break;

			case 2U:
			{
				// BLACKANDWHITE_ALPHA GRAYSCALE_ALPHA
				ASSERT_NYI();
			}
			break;

			case 3U:
			{
				if (max_value <= UINT8_MAX)
				{
					pnm_data<::lak::vec3u8_t, uint8_t> data;
					data.max_value = uint8_t(max_value);
					value          = ::lak::move(data);
				}
				else
				{
					pnm_data<::lak::vec3u16_t, uint16_t> data;
					data.max_value = uint16_t(max_value);
					value          = ::lak::move(data);
				}
			}
			break;

			case 4U:
			{
				if (max_value <= UINT8_MAX)
				{
					pnm_data<::lak::vec4u8_t, uint8_t> data;
					data.max_value = uint8_t(max_value);
					value          = ::lak::move(data);
				}
				else
				{
					pnm_data<::lak::vec4u16_t, uint16_t> data;
					data.max_value = uint16_t(max_value);
					value          = ::lak::move(data);
				}
			}
			break;

			default:
				ASSERT_UNREACHABLE();
		}
	}
	else
	{
		RES_TRY(
		  parser.read((*(lak::dsl::whitespace + lak::dsl::pound_line_comment))));

		RES_TRY_ASSIGN(width =, parser.read_u32(lak::dsl::dec_number));

		RES_TRY(parser.read((*lak::dsl::whitespace)));

		RES_TRY_ASSIGN(height =, parser.read_u32(lak::dsl::dec_number));

		if (type == pnm_type::Pf || type == pnm_type::PF || type == pnm_type::PF4)
		{
			RES_TRY(
			  parser.read((*(lak::dsl::whitespace + lak::dsl::pound_line_comment))));
			RES_TRY_ASSIGN(
			  float max_value =,
			  parser.read_f32<lak::dsl::signed_dec_number,
			                  lak::dsl::capture_nth<1U,
			                                        lak::dsl::char_literal<U'.'>,
			                                        lak::dsl::dec_number>,
			                  lak::dsl::bottom>());

			switch (type)
			{
				case pnm_type::Pf:
				{
					pnm_data<f32_t, f32_t> data;
					data.max_value = max_value;
					value          = ::lak::move(data);
				}
				break;

				case pnm_type::PF:
				{
					pnm_data<::lak::vec3f32_t, f32_t> data;
					data.max_value = max_value;
					value          = ::lak::move(data);
				}
				break;

				case pnm_type::PF4:
				{
					pnm_data<::lak::vec4f32_t, f32_t> data;
					data.max_value = max_value;
					value          = ::lak::move(data);
				}
				break;

				default:
					ASSERT_UNREACHABLE();
			}
		}
		else if (type == pnm_type::P1 || type == pnm_type::P4)
		{
			ascii_encoded = type == pnm_type::P1;
			pnm_data<bool, bool> data;
			data.max_value = true;
			value          = ::lak::move(data);
		}
		else
		{
			RES_TRY(
			  parser.read((*(lak::dsl::whitespace | lak::dsl::pound_line_comment))));

			RES_TRY_ASSIGN(uint16_t max_value =,
			               parser.read_u16(lak::dsl::dec_number));

			switch (type)
			{
				case pnm_type::P2:
					ascii_encoded = true;
					[[fallthrough]];
				case pnm_type::P5:
				{
					if (max_value <= UINT8_MAX)
					{
						pnm_data<uint8_t, uint8_t> data;
						data.max_value = uint8_t(max_value);
						value          = ::lak::move(data);
					}
					else
					{
						pnm_data<uint16_t, uint16_t> data;
						data.max_value = uint16_t(max_value);
						value          = ::lak::move(data);
					}
				}
				break;

				case pnm_type::P3:
					ascii_encoded = true;
					[[fallthrough]];
				case pnm_type::P6:
				{
					if (max_value <= UINT8_MAX)
					{
						pnm_data<::lak::vec3u8_t, uint8_t> data;
						data.max_value = uint8_t(max_value);
						value          = ::lak::move(data);
					}
					else
					{
						pnm_data<::lak::vec3u16_t, uint16_t> data;
						data.max_value = uint16_t(max_value);
						value          = ::lak::move(data);
					}
				}
				break;

				default:
					ASSERT_UNREACHABLE();
			}
		}

		if (!ascii_encoded)
		{
			RES_TRY(parser.read((*lak::dsl::ascii_nonnewline_whitespace) +
			                    lak::dsl::char_literal<U'\n'>));
		}
	}

	char c = 0;

	auto skip_whitespace = [&]() -> ::lak::error_code<::lak::err::out_of_data>
	{
		while (::lak::is_ascii_whitespace(c))
		{
			RES_TRY_ASSIGN(c =, strm.read_u8le());
		}
		return ::lak::ok_t{};
	};

	[[maybe_unused]] auto skip_comments =
	  [&]() -> ::lak::error_code<::lak::err::out_of_data>
	{
		RES_TRY(skip_whitespace());
		while (c == '#')
		{
			while (c != '\n')
			{
				RES_TRY_ASSIGN(c =, strm.read_u8le());
			}
			RES_TRY_ASSIGN(c =, strm.read_u8le());
			RES_TRY(skip_whitespace());
		}
		return ::lak::ok_t{};
	};

	auto read_integer = [&]<typename T>(uintmax_t max_v)
	  -> ::lak::result<T,
	                   ::lak::variant<lak::err::out_of_data,
	                                  lak::err::value_out_of_range,
	                                  lak::err::string_to_numeric,
	                                  lak::dsl::err::parse>>
	{
		RES_TRY(skip_whitespace());
		uintmax_t result = 0U;
		do
		{
			if (!::lak::is_alphanumeric(c))
				return ::lak::err_t{::lak::err::value_out_of_range{}};
			result *= 10;
			result += uint32_t(c - '0');
			if (result > max_v)
				return ::lak::err_t{::lak::err::value_out_of_range{}};
			if_let_ok (uint8_t u8 =, strm.read_u8le())
				c = char(u8);
			else
				c = 0;
		} while (::lak::is_alphanumeric(c));
		return ::lak::ok_t{static_cast<T>(result)};
	};

	auto res = value.visit(::lak::overloaded{
	  [](::lak::monostate &) -> ::lak::error_codes<lak::err::out_of_data,
	                                               lak::err::value_out_of_range,
	                                               lak::err::string_to_numeric,
	                                               lak::dsl::err::parse>
	  { return ::lak::err_t{::lak::err::value_out_of_range{}}; },
	  [&](pnm_data<bool, bool> &data)
	    -> ::lak::error_codes<lak::err::out_of_data,
	                          lak::err::value_out_of_range,
	                          lak::err::string_to_numeric,
	                          lak::dsl::err::parse>
	  {
		  data.resize(width * height);
		  if (ascii_encoded)
		  {
			  RES_TRY_ASSIGN(c =, strm.read_u8le());
			  for (auto &v : data)
			  {
				  RES_TRY_ASSIGN(auto _v =,
				                 read_integer.template operator()<uint8_t>(1U));
				  v = !!_v;
			  }
		  }
		  else
		  {
			  for (size_t y = 0U; y < height; ++y)
			  {
				  RES_TRY_ASSIGN(auto row =, strm.read_bytes((width + 7U) / 8U));
				  lak::bit_reader<lak::endian::big> row_reader{row};
				  for (size_t x = 0U; x < width; ++x)
				  {
					  RES_TRY_ASSIGN(
					    auto bits =,
					    row_reader.read_bits(1U).map_err(
					      [](auto &&err) -> lak::variant<lak::err::out_of_data,
					                                     lak::err::value_out_of_range,
					                                     lak::err::string_to_numeric,
					                                     lak::dsl::err::parse>
					      { return err; }));
					  data[(y * width) + x] = bool(bits);
				  }
			  }
		  }
		  return lak::ok_t{};
	  },
	  [&](pnm_data<uint8_t, uint8_t> &data)
	    -> ::lak::error_codes<lak::err::out_of_data,
	                          lak::err::value_out_of_range,
	                          lak::err::string_to_numeric,
	                          lak::dsl::err::parse>
	  {
		  data.resize(width * height);
		  if (ascii_encoded)
		  {
			  RES_TRY_ASSIGN(c =, strm.read_u8le());
			  for (auto &v : data)
			  {
				  RES_TRY_ASSIGN(
				    v =, read_integer.template operator()<uint8_t>(data.max_value));
			  }
		  }
		  else
		  {
			  for (auto &v : data)
			  {
				  RES_TRY_ASSIGN(v =, strm.read_u8be());
				  if (v > data.max_value)
					  return ::lak::err_t{::lak::err::value_out_of_range{}};
			  }
		  }
		  return lak::ok_t{};
	  },
	  [&](pnm_data<uint16_t, uint16_t> &data)
	    -> ::lak::error_codes<lak::err::out_of_data,
	                          lak::err::value_out_of_range,
	                          lak::err::string_to_numeric,
	                          lak::dsl::err::parse>
	  {
		  data.resize(width * height);
		  if (ascii_encoded)
		  {
			  RES_TRY_ASSIGN(c =, strm.read_u8le());
			  for (auto &v : data)
			  {
				  RES_TRY_ASSIGN(
				    v =, read_integer.template operator()<uint16_t>(data.max_value));
			  }
		  }
		  else
		  {
			  for (auto &v : data)
			  {
				  RES_TRY_ASSIGN(v =, strm.read_u16be());
				  if (v > data.max_value)
					  return ::lak::err_t{::lak::err::value_out_of_range{}};
			  }
		  }
		  return lak::ok_t{};
	  },
	  [&](pnm_data<f32_t, f32_t> &data)
	    -> ::lak::error_codes<lak::err::out_of_data,
	                          lak::err::value_out_of_range,
	                          lak::err::string_to_numeric,
	                          lak::dsl::err::parse>
	  {
		  ASSERT(!ascii_encoded);
		  data.resize(width * height);
		  if (std::signbit(data.max_value))
		  {
			  // little endian
			  data.max_value = std::abs(data.max_value);
			  for (auto &v : data)
			  {
				  RES_TRY_ASSIGN(v =, strm.read_f32le());
				  if (v < 0.0f || v > data.max_value)
					  return ::lak::err_t{::lak::err::value_out_of_range{}};
			  }
		  }
		  else
		  {
			  // big endian
			  for (auto &v : data)
			  {
				  RES_TRY_ASSIGN(v =, strm.read_f32be());
				  if (v < 0.0f || v > data.max_value)
					  return ::lak::err_t{::lak::err::value_out_of_range{}};
			  }
		  }
		  return lak::ok_t{};
	  },
	  [&](pnm_data<::lak::vec3u8_t, uint8_t> &data)
	    -> ::lak::error_codes<lak::err::out_of_data,
	                          lak::err::value_out_of_range,
	                          lak::err::string_to_numeric,
	                          lak::dsl::err::parse>
	  {
		  data.resize(width * height);
		  if (ascii_encoded)
		  {
			  RES_TRY_ASSIGN(c =, strm.read_u8le());
			  for (auto &v : data)
			  {
				  RES_TRY_ASSIGN(
				    v.r =, read_integer.template operator()<uint8_t>(data.max_value));
				  RES_TRY_ASSIGN(
				    v.g =, read_integer.template operator()<uint8_t>(data.max_value));
				  RES_TRY_ASSIGN(
				    v.b =, read_integer.template operator()<uint8_t>(data.max_value));
			  }
		  }
		  else
		  {
			  for (auto &v : data)
			  {
				  RES_TRY_ASSIGN(v.r =, strm.read_u8be());
				  if (v.r > data.max_value)
					  return ::lak::err_t{::lak::err::value_out_of_range{}};
				  RES_TRY_ASSIGN(v.g =, strm.read_u8be());
				  if (v.g > data.max_value)
					  return ::lak::err_t{::lak::err::value_out_of_range{}};
				  RES_TRY_ASSIGN(v.b =, strm.read_u8be());
				  if (v.b > data.max_value)
					  return ::lak::err_t{::lak::err::value_out_of_range{}};
			  }
		  }
		  return lak::ok_t{};
	  },
	  [&](pnm_data<::lak::vec4u8_t, uint8_t> &data)
	    -> ::lak::error_codes<lak::err::out_of_data,
	                          lak::err::value_out_of_range,
	                          lak::err::string_to_numeric,
	                          lak::dsl::err::parse>
	  {
		  data.resize(width * height);
		  if (ascii_encoded)
		  {
			  RES_TRY_ASSIGN(c =, strm.read_u8le());
			  for (auto &v : data)
			  {
				  RES_TRY_ASSIGN(
				    v.r =, read_integer.template operator()<uint8_t>(data.max_value));
				  RES_TRY_ASSIGN(
				    v.g =, read_integer.template operator()<uint8_t>(data.max_value));
				  RES_TRY_ASSIGN(
				    v.b =, read_integer.template operator()<uint8_t>(data.max_value));
				  RES_TRY_ASSIGN(
				    v.a =, read_integer.template operator()<uint8_t>(data.max_value));
			  }
		  }
		  else
		  {
			  for (auto &v : data)
			  {
				  RES_TRY_ASSIGN(v.r =, strm.read_u8be());
				  if (v.r > data.max_value)
					  return ::lak::err_t{::lak::err::value_out_of_range{}};
				  RES_TRY_ASSIGN(v.g =, strm.read_u8be());
				  if (v.g > data.max_value)
					  return ::lak::err_t{::lak::err::value_out_of_range{}};
				  RES_TRY_ASSIGN(v.b =, strm.read_u8be());
				  if (v.b > data.max_value)
					  return ::lak::err_t{::lak::err::value_out_of_range{}};
				  RES_TRY_ASSIGN(v.a =, strm.read_u8be());
				  if (v.a > data.max_value)
					  return ::lak::err_t{::lak::err::value_out_of_range{}};
			  }
		  }
		  return lak::ok_t{};
	  },
	  [&](pnm_data<::lak::vec3u16_t, uint16_t> &data)
	    -> ::lak::error_codes<lak::err::out_of_data,
	                          lak::err::value_out_of_range,
	                          lak::err::string_to_numeric,
	                          lak::dsl::err::parse>
	  {
		  data.resize(width * height);
		  if (ascii_encoded)
		  {
			  RES_TRY_ASSIGN(c =, strm.read_u8le());
			  for (auto &v : data)
			  {
				  RES_TRY_ASSIGN(
				    v.r =, read_integer.template operator()<uint16_t>(data.max_value));
				  RES_TRY_ASSIGN(
				    v.g =, read_integer.template operator()<uint16_t>(data.max_value));
				  RES_TRY_ASSIGN(
				    v.b =, read_integer.template operator()<uint16_t>(data.max_value));
			  }
		  }
		  else
		  {
			  for (auto &v : data)
			  {
				  RES_TRY_ASSIGN(v.r =, strm.read_u16be());
				  if (v.r > data.max_value)
					  return ::lak::err_t{::lak::err::value_out_of_range{}};
				  RES_TRY_ASSIGN(v.g =, strm.read_u16be());
				  if (v.g > data.max_value)
					  return ::lak::err_t{::lak::err::value_out_of_range{}};
				  RES_TRY_ASSIGN(v.b =, strm.read_u16be());
				  if (v.b > data.max_value)
					  return ::lak::err_t{::lak::err::value_out_of_range{}};
			  }
		  }
		  return lak::ok_t{};
	  },
	  [&](pnm_data<::lak::vec4u16_t, uint16_t> &data)
	    -> ::lak::error_codes<lak::err::out_of_data,
	                          lak::err::value_out_of_range,
	                          lak::err::string_to_numeric,
	                          lak::dsl::err::parse>
	  {
		  data.resize(width * height);
		  if (ascii_encoded)
		  {
			  RES_TRY_ASSIGN(c =, strm.read_u8le());
			  for (auto &v : data)
			  {
				  RES_TRY_ASSIGN(
				    v.r =, read_integer.template operator()<uint16_t>(data.max_value));
				  RES_TRY_ASSIGN(
				    v.g =, read_integer.template operator()<uint16_t>(data.max_value));
				  RES_TRY_ASSIGN(
				    v.b =, read_integer.template operator()<uint16_t>(data.max_value));
				  RES_TRY_ASSIGN(
				    v.a =, read_integer.template operator()<uint16_t>(data.max_value));
			  }
		  }
		  else
		  {
			  for (auto &v : data)
			  {
				  RES_TRY_ASSIGN(v.r =, strm.read_u16be());
				  if (v.r > data.max_value)
					  return ::lak::err_t{::lak::err::value_out_of_range{}};
				  RES_TRY_ASSIGN(v.g =, strm.read_u16be());
				  if (v.g > data.max_value)
					  return ::lak::err_t{::lak::err::value_out_of_range{}};
				  RES_TRY_ASSIGN(v.b =, strm.read_u16be());
				  if (v.b > data.max_value)
					  return ::lak::err_t{::lak::err::value_out_of_range{}};
				  RES_TRY_ASSIGN(v.a =, strm.read_u16be());
				  if (v.a > data.max_value)
					  return ::lak::err_t{::lak::err::value_out_of_range{}};
			  }
		  }
		  return lak::ok_t{};
	  },
	  [&](pnm_data<::lak::vec3f32_t, f32_t> &data)
	    -> ::lak::error_codes<lak::err::out_of_data,
	                          lak::err::value_out_of_range,
	                          lak::err::string_to_numeric,
	                          lak::dsl::err::parse>
	  {
		  ASSERT(!ascii_encoded);
		  data.resize(width * height);
		  if (std::signbit(data.max_value))
		  {
			  // little endian
			  data.max_value = std::abs(data.max_value);
			  for (auto &v : data)
			  {
				  RES_TRY_ASSIGN(v.r =, strm.read_f32le());
				  if (v.r < 0.0f || v.r > data.max_value)
					  return ::lak::err_t{::lak::err::value_out_of_range{}};
				  RES_TRY_ASSIGN(v.g =, strm.read_f32le());
				  if (v.g < 0.0f || v.g > data.max_value)
					  return ::lak::err_t{::lak::err::value_out_of_range{}};
				  RES_TRY_ASSIGN(v.b =, strm.read_f32le());
				  if (v.b < 0.0f || v.b > data.max_value)
					  return ::lak::err_t{::lak::err::value_out_of_range{}};
			  }
		  }
		  else
		  {
			  // big endian
			  for (auto &v : data)
			  {
				  RES_TRY_ASSIGN(v.r =, strm.read_f32be());
				  if (v.r < 0.0f || v.r > data.max_value)
					  return ::lak::err_t{::lak::err::value_out_of_range{}};
				  RES_TRY_ASSIGN(v.g =, strm.read_f32be());
				  if (v.g < 0.0f || v.g > data.max_value)
					  return ::lak::err_t{::lak::err::value_out_of_range{}};
				  RES_TRY_ASSIGN(v.b =, strm.read_f32be());
				  if (v.b < 0.0f || v.b > data.max_value)
					  return ::lak::err_t{::lak::err::value_out_of_range{}};
			  }
		  }
		  return lak::ok_t{};
	  },
	  [&](pnm_data<::lak::vec4f32_t, f32_t> &data)
	    -> ::lak::error_codes<lak::err::out_of_data,
	                          lak::err::value_out_of_range,
	                          lak::err::string_to_numeric,
	                          lak::dsl::err::parse>
	  {
		  ASSERT(!ascii_encoded);
		  data.resize(width * height);
		  if (std::signbit(data.max_value))
		  {
			  // little endian
			  data.max_value = std::abs(data.max_value);
			  for (auto &v : data)
			  {
				  RES_TRY_ASSIGN(v.r =, strm.read_f32le());
				  if (v.r < 0.0f || v.r > data.max_value)
					  return ::lak::err_t{::lak::err::value_out_of_range{}};
				  RES_TRY_ASSIGN(v.g =, strm.read_f32le());
				  if (v.g < 0.0f || v.g > data.max_value)
					  return ::lak::err_t{::lak::err::value_out_of_range{}};
				  RES_TRY_ASSIGN(v.b =, strm.read_f32le());
				  if (v.b < 0.0f || v.b > data.max_value)
					  return ::lak::err_t{::lak::err::value_out_of_range{}};
				  RES_TRY_ASSIGN(v.a =, strm.read_f32le());
				  if (v.a < 0.0f || v.a > data.max_value)
					  return ::lak::err_t{::lak::err::value_out_of_range{}};
			  }
		  }
		  else
		  {
			  // big endian
			  for (auto &v : data)
			  {
				  RES_TRY_ASSIGN(v.r =, strm.read_f32be());
				  if (v.r < 0.0f || v.r > data.max_value)
					  return ::lak::err_t{::lak::err::value_out_of_range{}};
				  RES_TRY_ASSIGN(v.g =, strm.read_f32be());
				  if (v.g < 0.0f || v.g > data.max_value)
					  return ::lak::err_t{::lak::err::value_out_of_range{}};
				  RES_TRY_ASSIGN(v.b =, strm.read_f32be());
				  if (v.b < 0.0f || v.b > data.max_value)
					  return ::lak::err_t{::lak::err::value_out_of_range{}};
				  RES_TRY_ASSIGN(v.a =, strm.read_f32be());
				  if (v.a < 0.0f || v.a > data.max_value)
					  return ::lak::err_t{::lak::err::value_out_of_range{}};
			  }
		  }
		  return lak::ok_t{};
	  },
	});

	RES_TRY(res);

	return lak::ok_t{};
}
