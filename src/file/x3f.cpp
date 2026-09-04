#include "lak/file/x3f.hpp"

#include "lak/bit_reader.hpp"
#include "lak/string_literals/span.hpp"
#include "lak/unique_ptr.hpp"

lak::error_code<lak::err::out_of_data> lak::x3f::header::_read(
  lak::binary_reader &strm)
{
	RES_TRYF_ASSIGN(section =,
	                strm.template read_le<lak::x3f::section_header>());
	if (section.version.major < 2U)
	{
		RES_TRYF_ASSIGN(versioned =,
		                strm.template read_le<lak::x3f::header_1_0>());
	}
	else if (section.version.major < 4U)
	{
		RES_TRYF_ASSIGN(versioned =,
		                strm.template read_le<lak::x3f::header_2_0>());
	}
	else
	{
		RES_TRYF_ASSIGN(versioned =,
		                strm.template read_le<lak::x3f::header_4_0>());
	}
	return lak::ok_t{};
}

lak::error_code<lak::err::out_of_data> lak::x3f::directory_2_0::_read(
  lak::binary_reader &strm)
{
	RES_TRYF_ASSIGN(uint32_t count =, strm.read_u32le());
	RES_TRYF_ASSIGN(entries =,
	                strm.template read_le<lak::x3f::directory_entry_2_0>(count));
	return lak::ok_t{};
}

lak::error_code<lak::err::out_of_data> lak::x3f::directory::_read(
  lak::binary_reader &strm)
{
	RES_TRYF_ASSIGN(section =,
	                strm.template read_le<lak::x3f::section_header>());
	RES_TRY(versioned.template emplace<lak::x3f::directory_2_0>()._read(strm));
	return lak::ok_t{};
}

struct huff_element
{
	lak::array<lak::unique_ptr<huff_element>, 2U> branch;
	lak::optional<uint8_t> leaf;
};

lak::result<huff_element,
            lak::variant<lak::err::out_of_data, lak::err::value_out_of_range>>
read_huff(lak::binary_reader &strm, bool right_align = true)
{
	huff_element huff;

	for (uint16_t i = 0U; i < 16; ++i)
	{
		RES_TRYF_ASSIGN(uint8_t len =, strm.read_u8le());
		RES_TRYF_ASSIGN(auto code_s =, strm.read_bytes(1U));
		if (len == 0U) break;
		lak::bit_reader<lak::endian::little, lak::endian::big> cstrm{code_s};
		huff_element *e = &huff;
		for (uint8_t j = 0U; j < len; ++j)
		{
			RES_TRYF_ASSIGN(auto pick =, cstrm.read_bits(1U));
			if (!e->branch[size_t(pick)])
				e->branch[size_t(pick)] = lak::unique_ptr<huff_element>::make();
			e = e->branch[size_t(pick)].get();
		}
		e->leaf = uint8_t(i);
	}

	return lak::move_ok(huff);
}

template<lak::endian EB, lak::endian Eb>
lak::bit_reader_result<uint32_t> read_true_diff(huff_element *e,
                                                lak::bit_reader<EB, Eb> &strm)
{
	while (e && (e->branch[0] || e->branch[1]))
	{
		RES_TRYF_ASSIGN(auto pick =, strm.read_bits(1U));
		e = e->branch[size_t(pick)].get();
	}
	if (!e)
	{
		ERROR("invalid huff path");
		return lak::ok_t{0U};
	}
	if (!e->leaf.has_value()) return lak::ok_t{0U};
	uint8_t len = *e->leaf;
	RES_TRYF_ASSIGN(uintmax_t diff =, strm.read_bits(len));
	if ((diff & uint32_t(1U << (len - 1U))) == 0)
		diff = uint32_t(int32_t(diff) - int32_t((1U << len) - 1U));
	return lak::ok_t{uint32_t(diff)};
}

lak::result<lak::array<size_t, 4U>, lak::err::out_of_data> read_sizes(
  lak::binary_reader &strm)
{
	lak::array<size_t, 4U> sizes;

	for (size_t i = 0; i < 4; ++i)
	{
		RES_TRYF_ASSIGN(sizes[i] =, strm.read_u32le());
	}

	return lak::move_ok(sizes);
}

#if 0
using ushort = unsigned short;
using uchar  = unsigned char;

auto getbithuff = [&](int nbits, const ushort *huff) -> unsigned
{
	static unsigned bitbuf = 0;
	static int vbits = 0, reset = 0;
	unsigned c;

	if (nbits == -1) return bitbuf = vbits = reset = 0;
	if (nbits == 0 || vbits < 0) return 0;
	while (vbits < nbits && (ifp < data.size()))
	{
		c      = char(data[ifp++]);
		bitbuf = (bitbuf << 8) + (uchar)c;
		vbits += 8;
	}
	c = bitbuf << (32 - vbits) >> (32 - nbits);
	if (huff)
	{
		vbits -= huff[c] >> 8;
		c = (uchar)huff[c];
	}
	else
		vbits -= nbits;
	return c;
};

#	define getbits(n) getbithuff(n, 0)
#	define gethuff(h) getbithuff(*h, h + 1)

auto ljpeg_diff = [&](ushort *huff) -> int
{
	int len, diff;

	len  = gethuff(huff);
	diff = getbits(len);
	if ((diff & (1 << (len - 1))) == 0) diff -= (1 << len) - 1;
	return diff;
};
#endif

lak::error_codes<lak::err::out_of_data, lak::err::value_out_of_range>
lak::x3f::image_data::_read(lak::binary_reader &strm)
{
	RES_TRYF_ASSIGN(section =,
	                strm.template read_le<lak::x3f::section_header>());
	RES_TRYF_ASSIGN(versioned =,
	                strm.template read_le<lak::x3f::image_data_2_0>());
	auto rem = strm.remaining();
	data     = lak::array<byte_t>(rem.begin(), rem.end());
	{
		auto res = versioned.visit(lak::overloaded{
		  [&](const lak::x3f::image_data_2_0 &d)
		    -> lak::error_codes<lak::err::out_of_data,
		                        lak::err::value_out_of_range>
		  {
			  if (d.type != lak::x3f::image_type::One &&
			      d.type != lak::x3f::image_type::Three)
				  return lak::ok_t{};
			  switch (d.format)
			  {
				  case lak::x3f::image_format(0x05): break;
				  case lak::x3f::image_format::SD9_SD10_SD14:
				  {
					  // if (true) return lak::ok_t{};
					  // RES_TRYF_ASSIGN(auto huff =, read_huff(strm));
					  // RES_TRYF_ASSIGN(auto roff =, read_roff(strm));

					  // lak::bit_reader bstrm{data};

					  RES_TRYF_ASSIGN(auto diff =,
					                  strm.template read_le<uint16_t>(1024));

					  image.resize({d.columns, d.rows});

					  if (/*load_flags*/ true)
					  {
						  for (uint32_t y = 0; y < d.rows; ++y)
						  {
							  int pred[] = {0, 0, 0, 0};
							  for (uint32_t x = 0; x < d.columns; ++x)
							  {
								  RES_TRYF_ASSIGN(uint32_t p =, strm.read_u32le());
								  for (size_t c = 0; c < 4; ++c)
									  pred[3 - c] += diff[p >> c * 10 & 0x3FF];
								  for (size_t c = 0; c < 4; ++c)
									  image[{x, y}][c] = uint16_t(pred[c]);
							  }
						  }
					  }
					  else
					  {
						  // foveon_decoder(1024, 0);
						  // int bit = -1;
						  for (uint32_t y = 0; y < d.rows; ++y)
						  {
							  int pred[] = {0, 0, 0};
							  // if (!bit && atoi(model + 2) < 14)
							  {
								  RES_TRY(strm.skip(4U));
							  }
							  for (uint32_t x = 0; x < d.columns; ++x)
							  {
								  // RES_TRYF_ASSIGN(uint32_t p =, strm.read_u32le());
								  for (size_t c = 0; c < 4; ++c)
								  {
									  // for (dindex = first_decode; dindex->branch[0];)
									  // {
									  //   if ((bit = (bit - 1) & 31) == 31)
									  // 	  for (i = 0; i < 4; i++)
									  // 		  bitbuf = (bitbuf << 8) + fgetc(ifp);
									  //   dindex = dindex->branch[bitbuf >> bit & 1];
									  // }
									  // pred[c] += diff[dindex->leaf];
									  // if (pred[c] >> 16 && ~pred[c] >> 16) derror();
								  }
								  for (size_t c = 0; c < 4; ++c)
									  image[{x, y}][c] = uint16_t(pred[c]);
							  }
						  }
					  }
				  }
				  break;

				  case lak::x3f::image_format::DP1_DP1S_DP2_Merril:
				  {
					  RES_TRYF_ASSIGN(auto seed =,
					                  strm.read_le<lak::array<uint16_t, 4U>>());
					  RES_TRYF_ASSIGN(auto huff =, read_huff(strm));
					  RES_TRYF_ASSIGN(auto sizes =,
					                  strm.read_le<lak::array<uint32_t, 4U>>());

					  image.resize({d.columns, d.rows});
					  image.fill(lak::vec4u16_t{0x0000U, 0x0000U, 0x0000U, 0xFFFFU});

					  lak::array<lak::span<const byte_t>, 3U> pdata;
					  for (size_t i = 0U; i < pdata.size(); ++i)
					  {
						  RES_TRYF_ASSIGN(
						    pdata[i] =,
						    strm.read_bytes(std::min<size_t>(
						      std::max<size_t>(strm.remaining().size(), sizes[i]),
						      lak::to_multiple<size_t>(sizes[i], 16U))));
					  }

					  auto thread_func =
					    [&](size_t i) -> lak::error_codes<lak::err::out_of_data,
					                                      lak::err::value_out_of_range>
					  {
						  lak::bit_reader<lak::endian::little, lak::endian::big> bstrm{
						    pdata[i]};
						  uint16_t vpred[2][2], hpred[2];
						  vpred[0][0] = vpred[0][1] = vpred[1][0] = vpred[1][1] = seed[i];
						  for (uint32_t y = 0; y < d.rows; y++)
						  {
							  for (uint32_t x = 0; x < d.columns; x++)
							  {
								  RES_TRYF_ASSIGN(uint32_t diff =,
								                  read_true_diff(&huff, bstrm));
								  if (x < 2)
									  hpred[x] = vpred[y & 1][x] += uint16_t(diff);
								  else
									  hpred[x & 1] += uint16_t(diff);
								  image[{x, y}][i] = hpred[x & 1] << (16U - 12U);
							  }
						  }
						  return lak::ok_t{};
					  };

#if 1
					  using res_type = lak::error_codes<lak::err::out_of_data,
					                                    lak::err::value_out_of_range>;
					  lak::array<res_type> results;
					  lak::threaded<void, res_type>(
					    [&](size_t i, lak::span<lak::atomic_optional<res_type>> outputs)
					    { outputs[i].emplace(thread_func(i)); },
					    [&](lak::span<lak::atomic_optional<res_type>> outputs)
					    {
						    results.reserve(outputs.size());
						    for (auto &out : outputs) results.emplace_back(out.release());
					    },
					    3U);
					  for (const auto &res : results) RES_TRYF(res);
#else
					  for (size_t i = 0U; i < 3U; ++i) RES_TRYF(thread_func(i));
#endif
				  }
				  break;
				  case lak::x3f::image_format::DP2_Quattro:  [[fallthrough]];
				  case lak::x3f::image_format::SD_Quattro:   [[fallthrough]];
				  case lak::x3f::image_format::SD_Quattro_H: [[fallthrough]];
				  case lak::x3f::image_format::SD_Quattro_H2:
				  {
					  RES_TRYF_ASSIGN(auto quattro_sizes =,
					                  strm.read_le<lak::array<lak::vec2u16_t, 3U>>());

					  [[maybe_unused]] bool quattro_layout =
					    quattro_sizes[0].x == d.columns / 2U;

					  RES_TRYF_ASSIGN(auto seed =,
					                  strm.read_le<lak::array<uint16_t, 4U>>());
					  DEBUG_EXPR(seed[0], seed[1], seed[2], seed[3]);
					  RES_TRYF_ASSIGN(auto huff =, read_huff(strm));
					  RES_TRYF_ASSIGN(auto sizes =,
					                  strm.read_le<lak::array<uint32_t, 4U>>());

					  image.resize({quattro_sizes[2].x, quattro_sizes[2].y});
					  image.fill(lak::vec4u16_t{0x0000U, 0x0000U, 0x0000U, 0xFFFFU});

					  DEBUG_EXPR(sizes[0], sizes[1], sizes[2], sizes[3]);
					  lak::array<lak::span<const byte_t>, 3U> pdata;
					  for (size_t i = 0U; i < pdata.size(); ++i)
					  {
						  DEBUG_EXPR(strm.position(), strm.remaining().size());
						  RES_TRYF_ASSIGN(
						    pdata[i] =,
						    strm.read_bytes(std::min<size_t>(
						      std::max<size_t>(strm.remaining().size(), sizes[i + 1U]),
						      lak::to_multiple<size_t>(sizes[i + 1U], 16U))));
					  }

					  auto thread_func =
					    [&](size_t i) -> lak::error_codes<lak::err::out_of_data,
					                                      lak::err::value_out_of_range>
					  {
						  lak::bit_reader<lak::endian::little, lak::endian::big> bstrm{
						    pdata[i]};
						  uint16_t vpred[2][2], hpred[2];
						  vpred[0][0] = vpred[0][1] = vpred[1][0] = vpred[1][1] = seed[i];
						  for (uint32_t y = 0; y < quattro_sizes[i].y; y++)
						  {
							  for (uint32_t x = 0; x < quattro_sizes[i].x; x++)
							  {
								  RES_TRYF_ASSIGN(uint32_t diff =,
								                  read_true_diff(&huff, bstrm));
								  if (x < 2)
									  hpred[x] = vpred[y & 1][x] += uint16_t(diff);
								  else
									  hpred[x & 1] += uint16_t(diff);

								  if (i == 2U)
									  image[{x, y}][i] = hpred[x & 1] << (16U - 12U);
								  else
									  for (uint32_t y2 = y * 2; y2 < (y * 2U) + 2U; ++y2)
										  for (uint32_t x2 = x * 2; x2 < (x * 2U) + 2U; ++x2)
											  image[{x2, y2}][i] = hpred[x & 1] << (16U - 11U);
							  }
						  }
						  return lak::ok_t{};
					  };

#if 0
					  using res_type = lak::error_codes<lak::err::out_of_data,
					                                    lak::err::value_out_of_range>;
					  lak::array<res_type> results;
					  lak::threaded<void, res_type>(
					    [&](size_t i, lak::span<lak::atomic_optional<res_type>> outputs)
					    { outputs[i].emplace(thread_func(i)); },
					    [&](lak::span<lak::atomic_optional<res_type>> outputs)
					    {
						    results.reserve(outputs.size());
						    for (auto &out : outputs) results.emplace_back(out.release());
					    },
					    3U);
					  for (const auto &res : results) RES_TRYF(res);
#else
					  for (size_t i = 0U; i < 3U; ++i) RES_TRYF(thread_func(i));
#endif
				  }
				  break;
			  }
			  return lak::ok_t{};
		  }});
		RES_TRY(res);
	}
	strm.read_remaining_bytes();
	return lak::ok_t{};
}

lak::error_codes<lak::err::out_of_data, lak::err::value_out_of_range>
lak::x3f::camf_data::_read(lak::binary_reader &strm)
{
	RES_TRYF_ASSIGN(section =,
	                strm.template read_le<lak::x3f::section_header>());
	RES_TRYF_ASSIGN(header =,
	                strm.template read_le<lak::x3f::camf_data_header>());
	auto rem = strm.read_remaining_bytes();
	data     = lak::array<byte_t>(rem.begin(), rem.end());

#if 0
	if (header.type == 2)
	{
		uint32_t h = header.rows;
		for (auto &d : data)
		{
			h            = (h * 1597 + 51749) % 244944;
			uint32_t val = h * (INT64)301593171 >> 24;
			d            = static_cast<byte_t>(static_cast<uint8_t>(d) ^
                              uint8_t(((((h << 8) - val) >> 1) + val) >> 17));
		}
	}
	else if (header.type == 4)
	{
		lak::binary_reader dstrm{data};

		RES_TRYF_ASSIGN(auto huff =, read_huff<258U>(dstrm));

		RES_TRY(dstrm.skip(4U));

		lak::bit_reader<lak::endian::little, lak::endian::big> bstrm{
		  dstrm.read_remaining_bytes()};

		auto _read_huff = [&]() -> lak::bit_reader_result<uint8_t>
		{
			RES_TRYF_ASSIGN(auto peeked =, bstrm.peek_bits(uint8_t(huff[0])));
			RES_TRY(bstrm.read_bits(uint8_t(huff[peeked + 1U] >> 8U)));
			return lak::ok_t{uint8_t(huff[peeked + 1U])};
		};

		auto read_diff = [&]() -> lak::bit_reader_result<uint32_t>
		{
			RES_TRYF_ASSIGN(uint8_t len =, _read_huff());
			DEBUG_EXPR(len);
			RES_TRYF_ASSIGN(uintmax_t diff =, bstrm.read_bits(len));
			if ((diff & uint32_t(1U << (len - 1U))) == 0)
				diff -= uint32_t((1U << len) - 1U);
			return lak::ok_t{uint32_t(diff)};
		};

		uint16_t vpred[2][2], hpred[2];
		lak::array<byte_t> camf;
		camf.resize((header.columns * header.rows * 3) / 2);

		for (uint32_t j = 0, y = 0; y < header.rows; y++)
		{
			for (uint32_t x = 0; x < header.columns; x++)
			{
				RES_TRYF_ASSIGN(uint32_t diff =, read_diff());
				if (x < 2)
					hpred[x] = vpred[y & 1][x] += static_cast<uint16_t>(diff);
				else
					hpred[x & 1] += static_cast<uint16_t>(diff);
				if (x & 1)
				{
					camf[j++] = static_cast<byte_t>(hpred[0] >> 4);
					camf[j++] = static_cast<byte_t>((hpred[0] << 4) | (hpred[1] >> 8));
					camf[j++] = static_cast<byte_t>(hpred[1]);
				}
			}
		}

		data = lak::move(camf);
	}
#endif

	return lak::ok_t{};
}

lak::error_codes<lak::err::out_of_data, lak::err::value_out_of_range>
lak::x3f::x3f::_read(lak::binary_reader &strm)
{
	RES_TRY(header._read(strm));
	RES_TRY(strm.seek(strm.size() - 4U));
	RES_TRYF_ASSIGN(uint32_t dir_ptr =, strm.read_u32le());
	RES_TRY(strm.seek(dir_ptr));
	{
		RES_TRYF_ASSIGN(auto sec_hdr =,
		                strm.template peek_le<lak::x3f::section_header>());
		if (lak::compare<char>(lak::span(sec_hdr.fourcc), "SECd"_span) != 4U)
		{
			ERROR("Corrupt X3F");
			return lak::err_t<lak::err::out_of_data>{};
		}
	}
	RES_TRY(directory._read(strm));
	{
		auto res = directory.versioned.visit(lak::overloaded{
		  [&](const lak::x3f::directory_2_0 &d)
		    -> lak::error_codes<lak::err::out_of_data,
		                        lak::err::value_out_of_range>
		  {
			  for (const auto &e : d.entries)
			  {
				  if (lak::compare<char>(e.type, "CAMF"_span) == 4U)
				  {
					  RES_TRY(strm.seek(e.offset));
					  {
						  RES_TRYF_ASSIGN(
						    auto sec_hdr =,
						    strm.template peek_le<lak::x3f::section_header>());
						  if (lak::compare<char>(lak::span(sec_hdr.fourcc), "SECc"_span) !=
						      4U)
						  {
							  ERROR("Corrupt X3F");
							  return lak::err_t<lak::err::out_of_data>{};
						  }
					  }
					  lak::span<const byte_t> cdata;
					  strm.seek(0U).unwrap();
					  RES_TRYF_ASSIGN(cdata =, strm.read_bytes(e.offset + e.size));
					  lak::binary_reader cstrm{cdata};
					  cstrm.seek(e.offset).unwrap();
					  lak::x3f::camf_data centry;
					  RES_TRY(centry._read(cstrm));
					  // RES_TRYF_ASSIGN(lak::x3f::camf_data centry =,
					  //                 cstrm.template
					  //                 read_le<lak::x3f::camf_data>());
					  camf_entries.push_back(lak::move(centry));
				  }
				  else if ((lak::compare<char>(e.type, "IMAG"_span) == 4U) ||
				           (lak::compare<char>(e.type, "IMA2"_span) == 4U))
				  {
					  RES_TRY(strm.seek(e.offset));

					  {
						  RES_TRYF_ASSIGN(
						    auto sec_hdr =,
						    strm.template peek_le<lak::x3f::section_header>());
						  if (lak::compare<char>(lak::span(sec_hdr.fourcc), "SECi"_span) !=
						      4U)
						  {
							  ERROR("Corrupt X3F");
							  return lak::err_t<lak::err::out_of_data>{};
						  }
					  }
					  lak::span<const byte_t> idata;
					  strm.seek(0U).unwrap();
					  RES_TRYF_ASSIGN(idata =, strm.read_bytes(e.offset + e.size));
					  lak::binary_reader istrm{idata};
					  istrm.seek(e.offset).unwrap();
					  lak::x3f::image_data ientry;
					  RES_TRY(ientry._read(istrm));
					  // RES_TRYF_ASSIGN(lak::x3f::image_data ientry =,
					  //                 istrm.template
					  //                 read_le<lak::x3f::image_data>());
					  ientry.offset = e.offset;
					  // RES_TRY(ientry.versioned.visit(lak::overloaded{
					  //   [&](const lak::x3f::image_data_2_0 &id)
					  //     -> lak::error_code<lak::err::out_of_data>
					  //   {
					  //     if (id.type != 0x2U || id.format != 0x12U) return
					  //     lak::ok_t{}; lak::binary_reader s{
					  //       lak::span<const byte_t>(ientry.data).subspan(12U)};
					  //     RES_TRYF_ASSIGN(auto tiff =,
					  //                    s.template read_le<lak::tiff::tiff>());
					  //     format_12s.push_back(lak::move(tiff));
					  //     return lak::ok_t{};
					  //   }}));
					  image_entries.push_back(lak::move(ientry));
				  }
			  }

			  return lak::ok_t{};
		  }});
		RES_TRY(res);
	}

	return lak::ok_t{};
}
