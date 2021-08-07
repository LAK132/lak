#ifndef LAK_TRIE_HPP
#define LAK_TRIE_HPP

#include "lak/span.hpp"
#include "lak/string.hpp"
#include "lak/string_view.hpp"

#include <optional>
#include <tuple>
#include <vector>

namespace lak
{
	template<typename CHAR, typename T>
	struct trie
	{
	private:
		lak::string<CHAR> _key;
		std::optional<T> _value;
		std::vector<char32_t> _map;
		std::vector<trie> _nodes;

	public:
		trie() = default;

		const lak::string<CHAR> &key() const { return _key; }
		const std::vector<char32_t> &map() const { return _map; }
		const std::vector<trie> &nodes() const { return _nodes; }

		// Value of this node.
		std::optional<T> &value();
		const std::optional<T> &value() const;

		// Find the node for key.
		trie *find(lak::string_view<CHAR> key);
		const trie *find(lak::string_view<CHAR> key) const;

		// Find the node for key, or create one.
		trie &find_or_emplace(const lak::string<CHAR> &key);
		trie &operator[](const lak::string<CHAR> &key);

		// Default construct a T at key if it does not already exist.
		void try_emplace(const lak::string<CHAR> &key);

		// Construct a T with args at key if it does not already exist.
		template<typename... ARGS>
		void try_emplace(const lak::string<CHAR> &key, ARGS &&...args);

		// Default construct a T at key, destroying the previous T if it already
		// existed.
		void force_emplace(const lak::string<CHAR> &key);

		// Construct a T with args at key, destroying the previous T if it already
		// existed.
		template<typename... ARGS>
		void force_emplace(const lak::string<CHAR> &key, ARGS &&...args);

	private:
		trie(lak::string<CHAR> &&k);

		trie(lak::string<CHAR> &&k, std::optional<T> &&v);

		trie(lak::string<CHAR> &&k,
		     std::vector<char32_t> &&m,
		     std::vector<trie> &&n);

		trie(lak::string<CHAR> &&k,
		     std::optional<T> &&v,
		     std::vector<char32_t> &&m,
		     std::vector<trie> &&n);

		trie *internal_try_emplace(lak::string_view<CHAR> key);

		trie *internal_force_emplace(lak::string_view<CHAR> key);

		static trie merge(trie &&node1, trie &&node2);

		static std::pair<trie *, lak::string_view<CHAR>> find(
		  trie *node, lak::string_view<CHAR> key);
	};

	template<typename T>
	using atrie = trie<char, T>;
	template<typename T>
	using wtrie = trie<wchar_t, T>;
	template<typename T>
	using u8trie = trie<char8_t, T>;
	template<typename T>
	using u16trie = trie<char16_t, T>;
	template<typename T>
	using u32trie = trie<char32_t, T>;
}

template<typename CHAR, typename T>
std::ostream &operator<<(std::ostream &strm, const lak::trie<CHAR, T> &trie);

#include "trie.inl"

#endif // LAK_TRIE_HPP
