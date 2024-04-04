#ifndef LAK_BLOB_SEARCH_HPP
#define LAK_BLOB_SEARCH_HPP
// Based on https://gist.github.com/asumagic/f1eea71cbe57e8d937caa401409e4df3

#include "lak/array.hpp"
#include "lak/bitset.hpp"
#include "lak/optional.hpp"
#include "lak/stdint.hpp"

namespace lak
{
	template<size_t CHUNK_SIZE = 4 * 1024 * 1024>
	struct blob_search
	{
	private:
		lak::span<const byte_t> _source;
		lak::array<lak::bitset<UINT16_MAX>> _value;

	public:
		blob_search() = default;
		inline blob_search(lak::span<const byte_t> source) { init(source); }

		static lak::bitset<UINT16_MAX> bitset_for_bytes(
		  lak::span<const byte_t> bytes);

		void init(lak::span<const byte_t> source);

		lak::optional<size_t> find(lak::span<const byte_t> bytes,
		                           size_t offset = 0U) const;

		inline size_t size() const { return _source.size(); }
		inline size_t empty() const { return _source.empty(); }
	};
}

#include "blob_search.inl"

#endif
