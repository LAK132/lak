#ifndef LAK_TRIE_HPP
#define LAK_TRIE_HPP

#include "lak/string.hpp"

#include <assert.h>
#include <optional>
#include <tuple>
#include <vector>

namespace lak
{
  template<typename T>
  struct trie
  {
    trie() = default;

    const lak::astring &key() const { return _key; }
    const lak::astring &map() const { return _map; }
    const std::vector<trie> &nodes() const { return _nodes; }

    // Value of this node.
    std::optional<T> &value();
    const std::optional<T> &value() const;

    // Find the node for key.
    trie *find(const lak::astring &key);
    const trie *find(const lak::astring &key) const;

    // Find the node for key, or create one.
    trie &find_or_emplace(const lak::astring &key);
    trie &operator[](const lak::astring &key);

    // Default construct a T at key if it does not already exist.
    void try_emplace(const lak::astring &key);

    // Construct a T with args at key if it does not already exist.
    template<typename... ARGS>
    void try_emplace(const lak::astring &key, ARGS &&... args);

    // Default construct a T at key, destroying the previous T if it already
    // existed.
    void force_emplace(const lak::astring &key);

    // Construct a T with args at key, destroying the previous T if it already
    // existed.
    template<typename... ARGS>
    void force_emplace(const lak::astring &key, ARGS &&... args);

  private:
    lak::astring _key;
    std::optional<T> _value;
    lak::astring _map        = "";
    std::vector<trie> _nodes = {};

    trie(lak::astring &&k);

    trie(lak::astring &&k, std::optional<T> &&v);

    trie(lak::astring &&k, lak::astring &&m, std::vector<trie> &&n);

    trie(lak::astring &&k,
         std::optional<T> &&v,
         lak::astring &&m,
         std::vector<trie> &&n);

    trie *internal_try_emplace(const lak::astring &key);

    trie *internal_force_emplace(const lak::astring &key);

    static trie merge(trie &&node1, trie &&node2);

    static std::pair<trie *, std::string> find(trie *node,
                                               std::string_view key);
  };
}

#include "trie.inl"

#endif // LAK_TRIE_HPP
