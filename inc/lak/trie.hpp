#ifndef LAK_TRIE_HPP
#define LAK_TRIE_HPP

#include "string.hpp"

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

    const std::string &key() const { return _key; }
    const std::string &map() const { return _map; }
    const std::vector<trie> &nodes() const { return _nodes; }

    // Value of this node.
    std::optional<T> &value();
    const std::optional<T> &value() const;

    // Find the node for key.
    trie *find(const std::string &key);
    const trie *find(const std::string &key) const;

    // Find the node for key, or create one.
    trie &find_or_emplace(const std::string &key);
    trie &operator[](const std::string &key);

    // Default construct a T at key if it does not already exist.
    void try_emplace(const std::string &key);

    // Construct a T with args at key if it does not already exist.
    template<typename... ARGS>
    void try_emplace(const std::string &key, ARGS &&... args);

    // Default construct a T at key, destroying the previous T if it already
    // existed.
    void force_emplace(const std::string &key);

    // Construct a T with args at key, destroying the previous T if it already
    // existed.
    template<typename... ARGS>
    void force_emplace(const std::string &key, ARGS &&... args);

  private:
    std::string _key;
    std::optional<T> _value;
    std::string _map         = "";
    std::vector<trie> _nodes = {};

    trie(std::string &&k);

    trie(std::string &&k, std::optional<T> &&v);

    trie(std::string &&k, std::string &&m, std::vector<trie> &&n);

    trie(
      std::string &&k,
      std::optional<T> &&v,
      std::string &&m,
      std::vector<trie> &&n);

    trie *internal_try_emplace(const std::string &key);

    trie *internal_force_emplace(const std::string &key);

    static trie merge(trie &&node1, trie &&node2);

    static std::pair<trie *, std::string> find(
      trie *node, std::string_view key);
  };
}

#include "trie.inl"

#endif // LAK_TRIE_HPP
