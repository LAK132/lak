namespace
{
  namespace impl
  {
    bool is_prefix(std::string_view str, std::string_view prefix)
    {
      return prefix.size() <= str.size() &&
             str.substr(0, prefix.size()) == prefix;
    }

    lak::astring longest_common_prefix(const lak::astring &str1,
                                       const lak::astring &str2)
    {
      return lak::astring(
        str1.begin(),
        std::mismatch(str1.begin(), str1.end(), str2.begin(), str2.end())
          .first);
    }
  }
}

template<typename T>
std::optional<T> &lak::trie<T>::value()
{
  return _value;
}

template<typename T>
const std::optional<T> &lak::trie<T>::value() const
{
  return _value;
}

template<typename T>
lak::trie<T> *lak::trie<T>::find(const lak::astring &key)
{
  auto [node, remaining_key] = find(this, key);
  return remaining_key == node->_key ? node : nullptr;
}

template<typename T>
const lak::trie<T> *lak::trie<T>::find(const lak::astring &key) const
{
  auto [node, remaining_key] = find(this, key);
  return remaining_key == node->_key ? node : nullptr;
}

template<typename T>
lak::trie<T> &lak::trie<T>::find_or_emplace(const lak::astring &key)
{
  auto [node, remaining_key] = find(this, key);

  if (remaining_key != node->_key)
  {
    node->try_emplace(remaining_key);
  }

  std::tie(node, remaining_key) = find(this, key);

  assert(remaining_key == node->_key);
  assert(node->_value);

  return *node;
}

template<typename T>
lak::trie<T> &lak::trie<T>::operator[](const lak::astring &key)
{
  return find_or_emplace(key);
}

template<typename T>
void lak::trie<T>::try_emplace(const lak::astring &key)
{
  if (auto node = internal_try_emplace(key); node) node->_value.emplace();
}

template<typename T>
template<typename... ARGS>
void lak::trie<T>::try_emplace(const lak::astring &key, ARGS &&... args)
{
  if (auto node = internal_try_emplace(key); node)
    node->_value.emplace(std::forward<ARGS...>(args...));
}

template<typename T>
void lak::trie<T>::force_emplace(const lak::astring &key)
{
  internal_force_emplace(key)->_value.emplace();
}

template<typename T>
template<typename... ARGS>
void lak::trie<T>::force_emplace(const lak::astring &key, ARGS &&... args)
{
  internal_force_emplace(key)->_value.emplace(std::forward<ARGS...>(args...));
}

template<typename T>
lak::trie<T>::trie(lak::astring &&k)
: _key(std::move(k)), _value(std::nullopt), _map(), _nodes()
{
}

template<typename T>
lak::trie<T>::trie(lak::astring &&k, std::optional<T> &&v)
: _key(std::move(k)), _value(std::move(v)), _map(), _nodes()
{
}

template<typename T>
lak::trie<T>::trie(lak::astring &&k, lak::astring &&m, std::vector<trie> &&n)
: _key(std::move(k)),
  _value(std::nullopt),
  _map(std::move(m)),
  _nodes(std::move(n))
{
}

template<typename T>
lak::trie<T>::trie(lak::astring &&k,
                   std::optional<T> &&v,
                   lak::astring &&m,
                   std::vector<trie> &&n)
: _key(std::move(k)),
  _value(std::move(v)),
  _map(std::move(m)),
  _nodes(std::move(n))
{
}

template<typename T>
lak::trie<T> *lak::trie<T>::internal_try_emplace(const lak::astring &key)
{
  auto [node, remaining_key] = find(this, key);

  if (remaining_key == node->_key)
  {
    // exact node already exists.
    return node->_value.has_value() ? nullptr : node;
  }

  // merge a newly constructed node with the terminal node that we found.
  *node = merge(std::move(*node), trie(lak::astring(remaining_key)));

  node = find(node, remaining_key).first;

  assert(!node->_value.has_value());

  return node;
}

template<typename T>
lak::trie<T> *lak::trie<T>::internal_force_emplace(const lak::astring &key)
{
  auto [node, remaining_key] = find(this, key);

  if (remaining_key == node->_key)
  {
    // exact node already exists.
    if (node->_value.has_value()) node->_value.reset();
    return node;
  }

  // merge a newly constructed node with the terminal node that we found.
  *node = merge(std::move(*node), trie(lak::astring(remaining_key)));

  node = find(node, remaining_key).first;

  assert(!node->_value.has_value());

  return node;
}

template<typename T>
lak::trie<T> lak::trie<T>::merge(lak::trie &&node1, lak::trie &&node2)
{
  assert(node1._key != node2._key);

  auto common = impl::longest_common_prefix(node1._key, node2._key);

  const auto common_length = common.size();

  if (common_length == node1._key.size())
  {
    // node1.key is a prefix of node2.key.
    node1._map += node2._key.at(common_length);
    node2._key = node2._key.substr(common_length + 1);
    node1._nodes.emplace_back(std::move(node2));
    return node1;
  }
  else if (common_length == node2._key.size())
  {
    // node2.key is a prefix of node1.key.
    node2._map += node1._key.at(common_length);
    node1._key = node1._key.substr(common_length + 1);
    node2._nodes.emplace_back(std::move(node1));
    return node2;
  }
  else
  {
    // node1.key and node2.key are divergent.
    lak::astring map = lak::astring() + node1._key.at(common_length) +
                       node2._key.at(common_length);

    node1._key = node1._key.substr(common_length + 1);
    node2._key = node2._key.substr(common_length + 1);

    std::vector<trie> nodes;
    nodes.emplace_back(std::move(node1));
    nodes.emplace_back(std::move(node2));

    return trie(std::move(common), std::move(map), std::move(nodes));
  }
}

template<typename T>
std::pair<lak::trie<T> *, std::string> lak::trie<T>::find(lak::trie *node,
                                                          std::string_view key)
{
  for (;;)
  {
    if (key.empty() || !impl::is_prefix(key, node->_key) || key == node->_key)
    {
      return {node, lak::astring(key)};
    }

    const auto common_size = node->_key.size();
    const auto lookup      = key.at(common_size);
    const auto pos         = node->_map.find_first_of(lookup);

    if (pos == std::string::npos)
    {
      return {node, lak::astring(key)};
    }
    else
    {
      node = node->_nodes.data() + pos;
      key  = key.substr(common_size + 1);
    }
  }
}
