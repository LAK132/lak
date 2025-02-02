#include "lak/structure/pnm.hpp"

#include "lak/bit_reader.hpp"
#include "lak/overloaded.hpp"
#include "lak/string_literals.hpp"

#include "lak/dsl/binary_reader.hpp"
#include "lak/dsl/dsl.hpp"
#include "lak/dsl/utility.hpp"

template<::lak::endian E>
::lak::error_codes<lak::out_of_data_error,
                   lak::value_out_of_range_error,
                   lak::string_to_numeric_error,
                   lak::dsl::parse_error>
lak::pnm::pnm::read(::lak::binary_reader &strm)
{
	lak::dsl::binary_reader parser{strm};

	pnm_type type;
	uint32_t height    = 0U;
	bool ascii_encoded = false;

	auto value_check = []<typename T>(uintmax_t value, T max_value)
	  -> lak::result<T,
	                 lak::variant<lak::out_of_data_error,
	                              lak::value_out_of_range_error,
	                              lak::string_to_numeric_error,
	                              lak::dsl::parse_error>>
	{
		if (value > max_value)
			return lak::err_t{::lak::value_out_of_range_error{}};
		else
			return lak::ok_t{static_cast<T>(value)};
	};

	auto type_parser = lak::dsl::replace_str_literal<u8"P1", pnm_type::P1> |
	                   lak::dsl::replace_str_literal<u8"P2", pnm_type::P2> |
	                   lak::dsl::replace_str_literal<u8"P3", pnm_type::P3> |
	                   lak::dsl::replace_str_literal<u8"P4", pnm_type::P4> |
	                   lak::dsl::replace_str_literal<u8"P5", pnm_type::P5> |
	                   lak::dsl::replace_str_literal<u8"P6", pnm_type::P6> |
	                   lak::dsl::replace_str_literal<u8"P7", pnm_type::P7> |
	                   lak::dsl::replace_str_literal<u8"PF4", pnm_type::PF4> |
	                   lak::dsl::replace_str_literal<u8"PF", pnm_type::PF> |
	                   lak::dsl::replace_str_literal<u8"Pf", pnm_type::Pf>;

	auto non_breaking_whitespace = (!U"\n"_dsl_char) & lak::dsl::whitespace;

	RES_TRY_ASSIGN(type =, parser.read(type_parser));

	RES_TRY(parser.read(*(lak::dsl::whitespace | lak::dsl::pound_line_comment)));

	if (type == pnm_type::P7)
	{
		enum struct tupltype_t
		{
			INVALID,
			BLACKANDWHITE,
			GRAYSCALE,
			RGB,
			BLACKANDWHITE_ALPHA,
			GRAYSCALE_ALPHA,
			RGB_ALPHA,
		} tupltype = tupltype_t::INVALID;

		auto tupltype_parser = lak::dsl::capture_nth<
		  1U,
		  u8"TUPLTYPE"_dsl_str + (+non_breaking_whitespace),
		  (lak::dsl::replace_str_literal<u8"BLACKANDWHITE",
		                                 tupltype_t::BLACKANDWHITE> |
		   lak::dsl::replace_str_literal<u8"GRAYSCALE", tupltype_t::GRAYSCALE> |
		   lak::dsl::replace_str_literal<u8"RGB", tupltype_t::RGB> |
		   lak::dsl::replace_str_literal<u8"BLACKANDWHITE_ALPHA",
		                                 tupltype_t::BLACKANDWHITE_ALPHA> |
		   lak::dsl::replace_str_literal<u8"GRAYSCALE_ALPHA",
		                                 tupltype_t::GRAYSCALE_ALPHA> |
		   lak::dsl::replace_str_literal<u8"RGB_ALPHA", tupltype_t::RGB_ALPHA>),
		  lak::dsl::next_char<U'\n'> +
		    (*(lak::dsl::pound_line_comment + lak::dsl::whitespace))>;

		auto width_parser =
		  lak::dsl::capture_nth<1U,
		                        u8"WIDTH"_dsl_str + (+non_breaking_whitespace),
		                        lak::dsl::parsed_dec_uint,
		                        lak::dsl::next_char<U'\n'> +
		                          (*(lak::dsl::pound_line_comment +
		                             lak::dsl::whitespace))>;

		auto height_parser =
		  lak::dsl::capture_nth<1U,
		                        u8"HEIGHT"_dsl_str + (+non_breaking_whitespace),
		                        lak::dsl::parsed_dec_uint,
		                        lak::dsl::next_char<U'\n'> +
		                          (*(lak::dsl::pound_line_comment +
		                             lak::dsl::whitespace))>;

		auto depth_parser =
		  lak::dsl::capture_nth<1U,
		                        u8"DEPTH"_dsl_str + (+non_breaking_whitespace),
		                        lak::dsl::parsed_dec_uint,
		                        lak::dsl::next_char<U'\n'> +
		                          (*(lak::dsl::pound_line_comment +
		                             lak::dsl::whitespace))>;

		auto maxval_parser =
		  lak::dsl::capture_nth<1U,
		                        u8"MAXVAL"_dsl_str + (+non_breaking_whitespace),
		                        lak::dsl::parsed_dec_uint,
		                        lak::dsl::next_char<U'\n'> +
		                          (*(lak::dsl::pound_line_comment +
		                             lak::dsl::whitespace))>;

		auto endhdr_parser = u8"ENDHDR"_dsl_str + lak::dsl::next_char<U'\n'>;

		auto header_parser = lak::dsl::capture_nth<
		  0U,
		  lak::dsl::unordered<width_parser,
		                      height_parser,
		                      depth_parser,
		                      maxval_parser,
		                      lak::dsl::optional<tupltype_parser>>,
		  endhdr_parser>;

		RES_TRY_ASSIGN(auto header =, parser.read(header_parser));

		RES_TRY_ASSIGN(width =,
		               header.get<0>().and_then(
		                 [&](uintmax_t value)
		                 { return value_check(value, uint32_t(UINT32_MAX)); }));

		RES_TRY_ASSIGN(height =,
		               header.get<1>().and_then(
		                 [&](uintmax_t value)
		                 { return value_check(value, uint32_t(UINT32_MAX)); }));

		RES_TRY_ASSIGN(
		  uint8_t depth =,
		  header.get<2>().and_then([&](uintmax_t value)
		                           { return value_check(value, uint8_t(4U)); }));

		RES_TRY_ASSIGN(uint16_t max_value =,
		               header.get<3>().and_then(
		                 [&](uintmax_t value)
		                 { return value_check(value, uint16_t(UINT16_MAX)); }));
		if (header.get<4>().has_value()) tupltype = *header.get<4>();

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
			  parser.read_f32<
			    lak::dsl::signed_dec_number,
			    lak::dsl::capture_nth<1U, U"."_dsl_char, lak::dsl::dec_number>,
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
	}

	char c = 0;

	auto skip_whitespace = [&]() -> ::lak::error_code<::lak::out_of_data_error>
	{
		while (::lak::is_ascii_whitespace(c))
		{
			RES_TRY_ASSIGN(c =, strm.read_u8le());
		}
		return ::lak::ok_t{};
	};

	auto skip_comments = [&]() -> ::lak::error_code<::lak::out_of_data_error>
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
	                   ::lak::variant<lak::out_of_data_error,
	                                  lak::value_out_of_range_error,
	                                  lak::string_to_numeric_error,
	                                  lak::dsl::parse_error>>
	{
		RES_TRY(skip_whitespace());
		uintmax_t result = 0U;
		do
		{
			if (!::lak::is_alphanumeric(c))
				return ::lak::err_t{::lak::value_out_of_range_error{}};
			result *= 10;
			result += uint32_t(c - '0');
			if (result > max_v)
				return ::lak::err_t{::lak::value_out_of_range_error{}};
			if_let_ok (uint8_t u8 =, strm.read_u8le())
				c = char(u8);
			else
				c = 0;
		} while (::lak::is_alphanumeric(c));
		return ::lak::ok_t{static_cast<T>(result)};
	};

	auto res = value.visit(::lak::overloaded{
	  [](::lak::monostate &) -> ::lak::error_codes<lak::out_of_data_error,
	                                               lak::value_out_of_range_error,
	                                               lak::string_to_numeric_error,
	                                               lak::dsl::parse_error>
	  { return ::lak::err_t{::lak::value_out_of_range_error{}}; },
	  [&](pnm_data<bool, bool> &data)
	    -> ::lak::error_codes<lak::out_of_data_error,
	                          lak::value_out_of_range_error,
	                          lak::string_to_numeric_error,
	                          lak::dsl::parse_error>
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
					      [](auto &&err) -> lak::variant<lak::out_of_data_error,
					                                     lak::value_out_of_range_error,
					                                     lak::string_to_numeric_error,
					                                     lak::dsl::parse_error>
					      { return err; }));
					  data[(y * width) + x] = bool(bits);
				  }
			  }
		  }
		  return lak::ok_t{};
	  },
	  [&](pnm_data<uint8_t, uint8_t> &data)
	    -> ::lak::error_codes<lak::out_of_data_error,
	                          lak::value_out_of_range_error,
	                          lak::string_to_numeric_error,
	                          lak::dsl::parse_error>
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
					  return ::lak::err_t{::lak::value_out_of_range_error{}};
			  }
		  }
		  return lak::ok_t{};
	  },
	  [&](pnm_data<uint16_t, uint16_t> &data)
	    -> ::lak::error_codes<lak::out_of_data_error,
	                          lak::value_out_of_range_error,
	                          lak::string_to_numeric_error,
	                          lak::dsl::parse_error>
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
					  return ::lak::err_t{::lak::value_out_of_range_error{}};
			  }
		  }
		  return lak::ok_t{};
	  },
	  [&](pnm_data<f32_t, f32_t> &data)
	    -> ::lak::error_codes<lak::out_of_data_error,
	                          lak::value_out_of_range_error,
	                          lak::string_to_numeric_error,
	                          lak::dsl::parse_error>
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
					  return ::lak::err_t{::lak::value_out_of_range_error{}};
			  }
		  }
		  else
		  {
			  // big endian
			  for (auto &v : data)
			  {
				  RES_TRY_ASSIGN(v =, strm.read_f32be());
				  if (v < 0.0f || v > data.max_value)
					  return ::lak::err_t{::lak::value_out_of_range_error{}};
			  }
		  }
		  return lak::ok_t{};
	  },
	  [&](pnm_data<::lak::vec3u8_t, uint8_t> &data)
	    -> ::lak::error_codes<lak::out_of_data_error,
	                          lak::value_out_of_range_error,
	                          lak::string_to_numeric_error,
	                          lak::dsl::parse_error>
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
					  return ::lak::err_t{::lak::value_out_of_range_error{}};
				  RES_TRY_ASSIGN(v.g =, strm.read_u8be());
				  if (v.g > data.max_value)
					  return ::lak::err_t{::lak::value_out_of_range_error{}};
				  RES_TRY_ASSIGN(v.b =, strm.read_u8be());
				  if (v.b > data.max_value)
					  return ::lak::err_t{::lak::value_out_of_range_error{}};
			  }
		  }
		  return lak::ok_t{};
	  },
	  [&](pnm_data<::lak::vec4u8_t, uint8_t> &data)
	    -> ::lak::error_codes<lak::out_of_data_error,
	                          lak::value_out_of_range_error,
	                          lak::string_to_numeric_error,
	                          lak::dsl::parse_error>
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
					  return ::lak::err_t{::lak::value_out_of_range_error{}};
				  RES_TRY_ASSIGN(v.g =, strm.read_u8be());
				  if (v.g > data.max_value)
					  return ::lak::err_t{::lak::value_out_of_range_error{}};
				  RES_TRY_ASSIGN(v.b =, strm.read_u8be());
				  if (v.b > data.max_value)
					  return ::lak::err_t{::lak::value_out_of_range_error{}};
				  RES_TRY_ASSIGN(v.a =, strm.read_u8be());
				  if (v.a > data.max_value)
					  return ::lak::err_t{::lak::value_out_of_range_error{}};
			  }
		  }
		  return lak::ok_t{};
	  },
	  [&](pnm_data<::lak::vec3u16_t, uint16_t> &data)
	    -> ::lak::error_codes<lak::out_of_data_error,
	                          lak::value_out_of_range_error,
	                          lak::string_to_numeric_error,
	                          lak::dsl::parse_error>
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
					  return ::lak::err_t{::lak::value_out_of_range_error{}};
				  RES_TRY_ASSIGN(v.g =, strm.read_u16be());
				  if (v.g > data.max_value)
					  return ::lak::err_t{::lak::value_out_of_range_error{}};
				  RES_TRY_ASSIGN(v.b =, strm.read_u16be());
				  if (v.b > data.max_value)
					  return ::lak::err_t{::lak::value_out_of_range_error{}};
			  }
		  }
		  return lak::ok_t{};
	  },
	  [&](pnm_data<::lak::vec4u16_t, uint16_t> &data)
	    -> ::lak::error_codes<lak::out_of_data_error,
	                          lak::value_out_of_range_error,
	                          lak::string_to_numeric_error,
	                          lak::dsl::parse_error>
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
					  return ::lak::err_t{::lak::value_out_of_range_error{}};
				  RES_TRY_ASSIGN(v.g =, strm.read_u16be());
				  if (v.g > data.max_value)
					  return ::lak::err_t{::lak::value_out_of_range_error{}};
				  RES_TRY_ASSIGN(v.b =, strm.read_u16be());
				  if (v.b > data.max_value)
					  return ::lak::err_t{::lak::value_out_of_range_error{}};
				  RES_TRY_ASSIGN(v.a =, strm.read_u16be());
				  if (v.a > data.max_value)
					  return ::lak::err_t{::lak::value_out_of_range_error{}};
			  }
		  }
		  return lak::ok_t{};
	  },
	  [&](pnm_data<::lak::vec3f32_t, f32_t> &data)
	    -> ::lak::error_codes<lak::out_of_data_error,
	                          lak::value_out_of_range_error,
	                          lak::string_to_numeric_error,
	                          lak::dsl::parse_error>
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
					  return ::lak::err_t{::lak::value_out_of_range_error{}};
				  RES_TRY_ASSIGN(v.g =, strm.read_f32le());
				  if (v.g < 0.0f || v.g > data.max_value)
					  return ::lak::err_t{::lak::value_out_of_range_error{}};
				  RES_TRY_ASSIGN(v.b =, strm.read_f32le());
				  if (v.b < 0.0f || v.b > data.max_value)
					  return ::lak::err_t{::lak::value_out_of_range_error{}};
			  }
		  }
		  else
		  {
			  // big endian
			  for (auto &v : data)
			  {
				  RES_TRY_ASSIGN(v.r =, strm.read_f32be());
				  if (v.r < 0.0f || v.r > data.max_value)
					  return ::lak::err_t{::lak::value_out_of_range_error{}};
				  RES_TRY_ASSIGN(v.g =, strm.read_f32be());
				  if (v.g < 0.0f || v.g > data.max_value)
					  return ::lak::err_t{::lak::value_out_of_range_error{}};
				  RES_TRY_ASSIGN(v.b =, strm.read_f32be());
				  if (v.b < 0.0f || v.b > data.max_value)
					  return ::lak::err_t{::lak::value_out_of_range_error{}};
			  }
		  }
		  return lak::ok_t{};
	  },
	  [&](pnm_data<::lak::vec4f32_t, f32_t> &data)
	    -> ::lak::error_codes<lak::out_of_data_error,
	                          lak::value_out_of_range_error,
	                          lak::string_to_numeric_error,
	                          lak::dsl::parse_error>
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
					  return ::lak::err_t{::lak::value_out_of_range_error{}};
				  RES_TRY_ASSIGN(v.g =, strm.read_f32le());
				  if (v.g < 0.0f || v.g > data.max_value)
					  return ::lak::err_t{::lak::value_out_of_range_error{}};
				  RES_TRY_ASSIGN(v.b =, strm.read_f32le());
				  if (v.b < 0.0f || v.b > data.max_value)
					  return ::lak::err_t{::lak::value_out_of_range_error{}};
				  RES_TRY_ASSIGN(v.a =, strm.read_f32le());
				  if (v.a < 0.0f || v.a > data.max_value)
					  return ::lak::err_t{::lak::value_out_of_range_error{}};
			  }
		  }
		  else
		  {
			  // big endian
			  for (auto &v : data)
			  {
				  RES_TRY_ASSIGN(v.r =, strm.read_f32be());
				  if (v.r < 0.0f || v.r > data.max_value)
					  return ::lak::err_t{::lak::value_out_of_range_error{}};
				  RES_TRY_ASSIGN(v.g =, strm.read_f32be());
				  if (v.g < 0.0f || v.g > data.max_value)
					  return ::lak::err_t{::lak::value_out_of_range_error{}};
				  RES_TRY_ASSIGN(v.b =, strm.read_f32be());
				  if (v.b < 0.0f || v.b > data.max_value)
					  return ::lak::err_t{::lak::value_out_of_range_error{}};
				  RES_TRY_ASSIGN(v.a =, strm.read_f32be());
				  if (v.a < 0.0f || v.a > data.max_value)
					  return ::lak::err_t{::lak::value_out_of_range_error{}};
			  }
		  }
		  return lak::ok_t{};
	  },
	});

	RES_TRY(res);

	return lak::ok_t{};
}
