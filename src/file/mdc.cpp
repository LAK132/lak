#include "lak/file/mdc.hpp"

#include "lak/algorithm.hpp"
#include "lak/binary_reader.hpp"
#include "lak/integer_range.hpp"
#include "lak/system/compiler.hpp"

lak::error_code<lak::err::out_of_data> lak::mdc::mdc::_read(
  lak::binary_reader &strm)
{
	RES_TRY_ASSIGN(preamble =, strm.read<uint8_t>(0x200U));

	RES_TRY_ASSIGN(
	  settings =,
	  lak::binary_reader{lak::span<const byte_t>(lak::span(preamble))}
	    .read<lak::mdc::mdc::shot_settings>());

	// sensors are 768 * 494 (0x300 * 0x1EE) pixels.
	// sections are 768 * 82 (0x300 * 0x52) bytes.

	constexpr size_t sector_size = 0x300U * 0x52U;

	lak::array<lak::span<const byte_t>, 7U> g1, rb, g2;

	for (size_t i = 0U; i < 6U; ++i)
	{
		RES_TRY_ASSIGN(g1[i] =, strm.read_bytes(sector_size));
		RES_TRY_ASSIGN(rb[i] =, strm.read_bytes(sector_size));
	}
	for (size_t i = 0U; i < 6U; ++i)
	{
		RES_TRY_ASSIGN(g2[i] =, strm.read_bytes(sector_size));
	}
	RES_TRY_ASSIGN(g1[6] =, strm.read_bytes(0x300U * 2));
	RES_TRY_ASSIGN(rb[6] =, strm.read_bytes(0x300U * 2));
	RES_TRY_ASSIGN(g2[6] =, strm.read_bytes(0x300U * 2));

	red.resize(0x180U * 0x1EEU);
	for (size_t y = 0U; y < (0x52U * 6U); ++y)
	{
		const size_t _y  = y * 0x180U;
		const size_t _y6 = (y / 6U) * 0x300U;
		for (size_t x = 0U; x < 0x180U; ++x)
		{
			red[x + _y] = uint8_t(rb[y % 6U][(x * 2U) + _y6]);
		}
	}
	for (size_t y = (0x52U * 6U); y < 0x1EEU; ++y)
	{
		const size_t _y  = y * 0x180U;
		const size_t _y6 = (y - (0x52U * 6U)) * 0x300U;
		for (size_t x = 0U; x < 0x180U; ++x)
		{
			red[x + _y] = uint8_t(rb[6][(x * 2U) + _y6]);
		}
	}

	green1.resize(0x300U * 0x1EEU);
	for (size_t y = 0U; y < (0x52U * 6U); ++y)
	{
		const size_t _y  = y * 0x300U;
		const size_t _y6 = (y / 6U) * 0x300U;
		for (size_t x = 0U; x < 0x300U; ++x)
		{
			green1[x + _y] = uint8_t(g1[y % 6U][x + _y6]);
		}
	}
	for (size_t y = (0x52U * 6U); y < 0x1EEU; ++y)
	{
		const size_t _y  = y * 0x300U;
		const size_t _y6 = (y - (0x52U * 6U)) * 0x300U;
		for (size_t x = 0U; x < 0x300U; ++x)
		{
			green1[x + _y] = uint8_t(g1[6][x + _y6]);
		}
	}

	green2.resize(0x300U * 0x1EEU);
	for (size_t y = 0U; y < (0x52U * 6U); ++y)
	{
		const size_t _y  = y * 0x300U;
		const size_t _y6 = (y / 6U) * 0x300U;
		for (size_t x = 0U; x < 0x300U; ++x)
		{
			green2[x + _y] = uint8_t(g2[y % 6U][x + _y6]);
		}
	}
	for (size_t y = (0x52U * 6U); y < 0x1EEU; ++y)
	{
		const size_t _y  = y * 0x300U;
		const size_t _y6 = (y - (0x52U * 6U)) * 0x300U;
		for (size_t x = 0U; x < 0x300U; ++x)
		{
			green2[x + _y] = uint8_t(g2[6][x + _y6]);
		}
	}

	blue.resize(0x180U * 0x1EEU);
	for (size_t y = 0U; y < (0x52U * 6U); ++y)
	{
		const size_t _y  = y * 0x180U;
		const size_t _y6 = (y / 6U) * 0x300U;
		for (size_t x = 0U; x < 0x180U; ++x)
		{
			blue[x + _y] = uint8_t(rb[y % 6U][1U + (x * 2U) + _y6]);
		}
	}
	for (size_t y = (0x52U * 6U); y < 0x1EEU; ++y)
	{
		const size_t _y  = y * 0x180U;
		const size_t _y6 = (y - (0x52U * 6U)) * 0x300U;
		for (size_t x = 0U; x < 0x180U; ++x)
		{
			blue[x + _y] = uint8_t(rb[6][1U + (x * 2U) + _y6]);
		}
	}

	return lak::ok_t{};
}

uint8_t lak::mdc::mdc::sample_channel(const lak::span<const uint8_t> data,
                                      const lak::vec2s_t source_size,
                                      lak::vec2s_t index) const
{
	BOUNDS_ASSERT_EQUAL(data.size(), (source_size.x * source_size.y));

	const auto t_x = uint32_t(index.x % 8U);
	const auto t_y = uint32_t(index.y % 8U);
	index          = index / size_t(8U);

	const size_t x1 = std::min(index.x, source_size.x - 1U);
	const size_t x2 = std::min(index.x + 1U, source_size.x - 1U);
	const size_t y1 = std::min(index.y, source_size.y - 1U);
	const size_t y2 = std::min(index.y + 1U, source_size.y - 1U);

	const auto d11 = data[x1 + (y1 * source_size.x)];
	const auto d21 = data[x2 + (y1 * source_size.x)];
	const uint32_t dt1 =
	  ((uint32_t(d11) * (8U - t_x)) + (uint32_t(d21) * t_x)) / 8U;

	const auto d12 = data[x1 + (y2 * source_size.x)];
	const auto d22 = data[x2 + (y2 * source_size.x)];
	const uint32_t dt2 =
	  ((uint32_t(d12) * (8U - t_x)) + (uint32_t(d22) * t_x)) / 8U;

	return uint8_t(((uint32_t(dt1) * (8U - t_y)) + (uint32_t(dt2) * t_y)) / 8U);
}

uint8_t lak::mdc::mdc::sample_r(const lak::vec2s_t index) const
{
	return sample_channel(red, red_blue_data_size, index);
}

uint8_t lak::mdc::mdc::sample_g1(const lak::vec2s_t index) const
{
	return sample_channel(green1, green_data_size, index);
}

uint8_t lak::mdc::mdc::sample_g2(const lak::vec2s_t index) const
{
	return sample_channel(green2, green_data_size, index);
}

uint8_t lak::mdc::mdc::sample_b(const lak::vec2s_t index) const
{
	return sample_channel(blue, red_blue_data_size, index);
}

lak::vec3u8_t lak::mdc::mdc::sample(const lak::vec2s_t r_pos,
                                    const lak::vec2s_t g1_pos,
                                    const lak::vec2s_t g2_pos,
                                    const lak::vec2s_t b_pos) const
{
	// :TODO: better interpolate g1 and g2.
	return {
	  sample_r(r_pos),
	  uint8_t((uint16_t(sample_g1(g1_pos)) + uint16_t(sample_g2(g2_pos))) / 2U),
	  sample_b(b_pos)};
}

lak::vec3u8_t lak::mdc::mdc::process_sample(const lak::vec2s_t xy) const
{
	const size_t _y   = size_t(((uint64_t(xy.y) * 10U * 8U) / aspect_ratio.y) +
                           image_inset.y + image_offset.y);
	const size_t r_y  = red_offset.y < 0
	                      ? _y - std::min(_y, size_t(-red_offset.y))
	                      : _y + size_t(red_offset.y);
	const size_t g1_y = green1_offset.y < 0
	                      ? _y - std::min(_y, size_t(-green1_offset.y))
	                      : _y + size_t(green1_offset.y);
	const size_t g2_y = green2_offset.y < 0
	                      ? _y - std::min(_y, size_t(-green2_offset.y))
	                      : _y + size_t(green2_offset.y);
	const size_t b_y  = blue_offset.y < 0
	                      ? _y - std::min(_y, size_t(-blue_offset.y))
	                      : _y + size_t(blue_offset.y);

	const size_t _x = size_t(((uint64_t(xy.x) * 10U * 8U) / aspect_ratio.x) +
	                         image_inset.x + image_offset.x);
	const size_t r_x =
	  (red_offset.x < 0 ? _y - std::min(_x, size_t(-red_offset.x))
	                    : _x + size_t(red_offset.x)) /
	  2U;
	const size_t g1_x = green1_offset.x < 0
	                      ? _y - std::min(_x, size_t(-green1_offset.x))
	                      : _x + size_t(green1_offset.x);
	const size_t g2_x = green2_offset.x < 0
	                      ? _y - std::min(_x, size_t(-green2_offset.x))
	                      : _x + size_t(green2_offset.x);
	const size_t b_x =
	  (blue_offset.x < 0 ? _y - std::min(_x, size_t(-blue_offset.x))
	                     : _x + size_t(blue_offset.x)) /
	  2U;

	return sample({r_x, r_y}, {g1_x, g1_y}, {g2_x, g2_y}, {b_x, b_y});
}

lak::vec3u8_t lak::mdc::mdc::get_pixel(lak::vec2s_t coord) const
{
	ASSERT_LESS(coord.x, upscaled_image_size.x);
	ASSERT_LESS(coord.y, upscaled_image_size.y);
	ASSERT_EQUAL(red.size(), red_blue_data_size.x * red_blue_data_size.y);
	ASSERT_EQUAL(green1.size(), green_data_size.x * green_data_size.y);
	ASSERT_EQUAL(green2.size(), green_data_size.x * green_data_size.y);
	ASSERT_EQUAL(blue.size(), red_blue_data_size.x * red_blue_data_size.y);

	return process_sample(coord);
}

lak::image<lak::color3_t> lak::mdc::mdc::process_color3() const
{
	ASSERT_EQUAL(red.size(), red_blue_data_size.x * red_blue_data_size.y);
	ASSERT_EQUAL(green1.size(), green_data_size.x * green_data_size.y);
	ASSERT_EQUAL(green2.size(), green_data_size.x * green_data_size.y);
	ASSERT_EQUAL(blue.size(), red_blue_data_size.x * red_blue_data_size.y);

	auto vec3_to_color3 = [](lak::vec3u8_t c) -> lak::color3_t
	{ return {c.x, c.y, c.z}; };

	lak::image<lak::color3_t> result;
	result.resize(upscaled_image_size);
	for (size_t y = 0U; y < result.size().y; ++y)
		for (size_t x = 0U; x < result.size().x; ++x)
			result[{x, y}] = vec3_to_color3(process_sample({x, y}));
	return result;
}

lak::image<lak::color3_t> lak::mdc::mdc::process_color3(
  lak::tasks &tasks) const
{
	ASSERT_EQUAL(red.size(), red_blue_data_size.x * red_blue_data_size.y);
	ASSERT_EQUAL(green1.size(), green_data_size.x * green_data_size.y);
	ASSERT_EQUAL(green2.size(), green_data_size.x * green_data_size.y);
	ASSERT_EQUAL(blue.size(), red_blue_data_size.x * red_blue_data_size.y);

	auto vec3_to_color3 = [](lak::vec3u8_t c) -> lak::color3_t
	{ return {c.x, c.y, c.z}; };

	lak::image<lak::color3_t> result;
	result.resize(upscaled_image_size);
	for (size_t y = 0U; y < result.size().y; ++y)
		tasks.push(
		  [&, y = y]()
		  {
			  for (size_t x = 0U; x < result.size().x; ++x)
				  result[{x, y}] = vec3_to_color3(process_sample({x, y}));
		  });
	return result;
}

lak::image<lak::vec3u8_t> lak::mdc::mdc::process_vec3u8() const
{
	ASSERT_EQUAL(red.size(), red_blue_data_size.x * red_blue_data_size.y);
	ASSERT_EQUAL(green1.size(), green_data_size.x * green_data_size.y);
	ASSERT_EQUAL(green2.size(), green_data_size.x * green_data_size.y);
	ASSERT_EQUAL(blue.size(), red_blue_data_size.x * red_blue_data_size.y);

	lak::image<lak::vec3u8_t> result;
	result.resize(upscaled_image_size);
	for (size_t y = 0U; y < result.size().y; ++y)
		for (size_t x = 0U; x < result.size().x; ++x)
			result[{x, y}] = process_sample({x, y});
	return result;
}

lak::image<lak::vec3u8_t> lak::mdc::mdc::process_vec3u8(
  lak::tasks &tasks) const
{
	ASSERT_EQUAL(red.size(), red_blue_data_size.x * red_blue_data_size.y);
	ASSERT_EQUAL(green1.size(), green_data_size.x * green_data_size.y);
	ASSERT_EQUAL(green2.size(), green_data_size.x * green_data_size.y);
	ASSERT_EQUAL(blue.size(), red_blue_data_size.x * red_blue_data_size.y);

	lak::image<lak::vec3u8_t> result;
	result.resize(upscaled_image_size);
	for (size_t y = 0U; y < result.size().y; ++y)
		tasks.push(
		  [&, y = y]()
		  {
			  for (size_t x = 0U; x < result.size().x; ++x)
				  result[{x, y}] = process_sample({x, y});
		  });
	return result;
}

lak::image<lak::vec3f_t> lak::mdc::mdc::process_vec3f() const
{
	ASSERT_EQUAL(red.size(), red_blue_data_size.x * red_blue_data_size.y);
	ASSERT_EQUAL(green1.size(), green_data_size.x * green_data_size.y);
	ASSERT_EQUAL(green2.size(), green_data_size.x * green_data_size.y);
	ASSERT_EQUAL(blue.size(), red_blue_data_size.x * red_blue_data_size.y);

	auto vec3u8_to_vec3f = [](lak::vec3u8_t c) -> lak::vec3f_t
	{
		return {lak::int_to_frac<float>(c.x),
		        lak::int_to_frac<float>(c.y),
		        lak::int_to_frac<float>(c.z)};
	};

	lak::image<lak::vec3f_t> result;
	result.resize(upscaled_image_size);
	for (size_t y = 0U; y < result.size().y; ++y)
		for (size_t x = 0U; x < result.size().x; ++x)
			result[{x, y}] = vec3u8_to_vec3f(process_sample({x, y}));
	return result;
}

lak::image<lak::vec3f_t> lak::mdc::mdc::process_vec3f(lak::tasks &tasks) const
{
	ASSERT_EQUAL(red.size(), red_blue_data_size.x * red_blue_data_size.y);
	ASSERT_EQUAL(green1.size(), green_data_size.x * green_data_size.y);
	ASSERT_EQUAL(green2.size(), green_data_size.x * green_data_size.y);
	ASSERT_EQUAL(blue.size(), red_blue_data_size.x * red_blue_data_size.y);

	auto vec3u8_to_vec3f = [](lak::vec3u8_t c) -> lak::vec3f_t
	{
		return {lak::int_to_frac<float>(c.x),
		        lak::int_to_frac<float>(c.y),
		        lak::int_to_frac<float>(c.z)};
	};

	lak::image<lak::vec3f_t> result;
	result.resize(upscaled_image_size);
	for (size_t y = 0U; y < result.size().y; ++y)
		tasks.push(
		  [&, y = y]()
		  {
			  for (size_t x = 0U; x < result.size().x; ++x)
				  result[{x, y}] = vec3u8_to_vec3f(process_sample({x, y}));
		  });
	return result;
}
