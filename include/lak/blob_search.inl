#include "lak/blob_search.hpp"

template<size_t CHUNK_SIZE>
lak::bitset<UINT16_MAX> lak::blob_search<CHUNK_SIZE>::bitset_for_bytes(
  lak::span<const byte_t> bytes)
{
	lak::bitset<UINT16_MAX> result;
	for (size_t i = 1U; i < bytes.size(); ++i)
		result.set(uint16_t(bytes[i - 1U]) | (uint16_t(bytes[i]) << 8U), true);
	return result;
}

template<size_t CHUNK_SIZE>
void lak::blob_search<CHUNK_SIZE>::init(lak::span<const byte_t> source)
{
	_value.clear();
	_source = source;
	if (_source.empty()) return;

	_value.reserve(lak::ceil_div(source.size(), CHUNK_SIZE));

	while (!source.empty())
	{
		_value.push_back(bitset_for_bytes(
		  source.first(std::min<size_t>(source.size(), CHUNK_SIZE + 1U))));
		source = source.subspan(std::min<size_t>(source.size(), CHUNK_SIZE));
	}
}

template<size_t CHUNK_SIZE>
lak::optional<size_t> lak::blob_search<CHUNK_SIZE>::find(
  lak::span<const byte_t> bytes, size_t offset) const
{
	if (bytes.size() == 0 || _source.empty()) return lak::nullopt;

	if (bytes.size() == 1)
	{
		for (size_t i = offset; i < _source.size(); ++i)
			if (_source[i] == bytes[0]) return lak::some_t{i};
	}
	else if (bytes.size() == 2)
	{
		uint16_t find = uint16_t(bytes[0]) | (uint16_t(bytes[1]) << 8U);
		for (size_t i = offset / CHUNK_SIZE; i < _value.size(); ++i)
		{
			if (!_value[i].get(find)) continue;

			auto subspan = _source.subspan(std::min(i * CHUNK_SIZE, _source.size()));
			subspan      = subspan.first(std::min(subspan.size(), CHUNK_SIZE + 1U));
			if (offset > (i * CHUNK_SIZE)) [[unlikely]]
				subspan =
				  subspan.subspan(std::min(offset - (i * CHUNK_SIZE), subspan.size()));
			if (auto res = lak::find_subspan(subspan, bytes); !res.empty())
				return lak::some_t{size_t(res.begin() - _source.begin())};
		}
	}
	else
	{
		uint16_t find_start     = uint16_t(bytes[0]) | (uint16_t(bytes[1]) << 8U);
		const auto find_set     = bitset_for_bytes(bytes);
		const size_t max_chunks = lak::ceil_div(bytes.size(), CHUNK_SIZE) + 1U;
		for (size_t i = offset / CHUNK_SIZE;
		     i <= _value.size() - (max_chunks - 1U);
		     ++i)
		{
			if (!_value[i].get(find_start)) continue;

			lak::bitset check_set = _value[i];
			for (size_t j = 1; j < max_chunks && i + j < _value.size(); ++j)
				check_set |= _value[i + j];

			if ((check_set & find_set) != find_set) continue;

			// all the right pairs are here, do a more thorough search.
			auto subspan = _source.subspan(std::min(i * CHUNK_SIZE, _source.size()));
			subspan      = subspan.first(
        std::min(subspan.size(), (max_chunks * CHUNK_SIZE) + 1U));
			if (offset > (i * CHUNK_SIZE)) [[unlikely]]
				subspan =
				  subspan.subspan(std::min(offset - (i * CHUNK_SIZE), subspan.size()));
			if (auto res = lak::find_subspan(subspan, bytes); !res.empty())
				return lak::some_t{size_t(res.begin() - _source.begin())};
		}
	}

	return lak::nullopt;
}
