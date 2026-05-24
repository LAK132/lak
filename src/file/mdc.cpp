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

lak::mdc::mdc::operator lak::image3_t() const
{
	const lak::vec2s_t sensor_size{768U, 494U};
	const lak::vec2s_t aspect_ratio{25U, 29U};
	const lak::vec2s_t inset{4U, 2U};
	const lak::vec2s_t offset{8U, 0U};
	lak::image3_t result;
	// 768 x 494
	// - 2*inset      = 760 x 490
	// * aspect_ratio = 19000 x 14210
	// / 10           = 1900 x 1421
	result.resize(((sensor_size - (inset * size_t(2U))) * aspect_ratio) /
	              size_t(10U));

	auto sample_channel = [](const lak::span<const uint8_t> data,
	                         const lak::vec2s_t source_size,
	                         lak::vec2s_t index) -> uint8_t
	{
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

		return uint8_t(((uint32_t(dt1) * (8U - t_y)) + (uint32_t(dt2) * t_y)) /
		               8U);
	};

	auto sample = [&, this](const lak::vec2s_t r_pos,
	                        const lak::vec2s_t g1_pos,
	                        const lak::vec2s_t g2_pos,
	                        const lak::vec2s_t b_pos) -> lak::color3_t
	{
		const auto r_sample  = sample_channel(red, {0x180U, 0x1EEU}, r_pos);
		const auto g1_sample = sample_channel(green1, {0x300U, 0x1EEU}, g1_pos);
		const auto g2_sample = sample_channel(green2, {0x300U, 0x1EEU}, g2_pos);
		const auto b_sample  = sample_channel(blue, {0x180U, 0x1EEU}, b_pos);
		// :TODO: better interpolate g1 and g2.
		return {r_sample,
		        uint8_t((uint16_t(g1_sample) + uint16_t(g2_sample)) / 2U),
		        b_sample};
	};

	auto process_sample = [&](const lak::vec2s_t xy) -> lak::color3_t
	{
		const size_t _y   = size_t(((uint64_t(xy.y) * 10U * 8U) / aspect_ratio.y) +
                             inset.y + offset.y);
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
		                         inset.x + offset.x);
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
	};

#if 0
	for (size_t y = 0U; y < result.size().y; ++y)
		for (size_t x = 0U; x < result.size().x; ++x)
			result[{x, y}] = process_sample({x, y});
#else
	lak::threaded<lak::pair<size_t, lak::span<lak::color3_t>>>(
	  [&](size_t id, auto inputs)
	  {
		  lak::while_some([&]() { return inputs[id].try_release(); },
		                  [&](lak::pair<size_t, lak::span<lak::color3_t>> data)
		                  {
			                  auto [y, d] = data;
			                  for (lak::vec2s_t xy{0U, y}; xy.x < d.size(); ++xy.x)
				                  d[xy.x] = process_sample(xy);
		                  });
	  },
	  [&](auto inputs)
	  {
		  auto data           = lak::span(result.data(), result.contig_size());
		  const size_t stride = result.size().x;
		  for (size_t i = 0U, y = 0U; y < result.size().y;
		       ++y, i           = (i + 1U) % inputs.size())
        inputs[i].emplace(y, data.subspan(y * stride, stride));
		  for (auto &in : inputs) in.await_none();
	  });
#endif

	return result;
}
