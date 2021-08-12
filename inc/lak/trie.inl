#include "lak/debug.hpp"
#include "lak/string_view.hpp"
#include "lak/utility.hpp"

namespace
{
	namespace impl
	{
		template<typename CHAR>
		bool is_prefix(lak::string_view<CHAR> str, lak::string_view<CHAR> prefix)
		{
			return prefix.size() < str.size() && str.first(prefix.size()) == prefix;
		}
	}
}

template<typename CHAR, typename T>
std::optional<T> &lak::trie<CHAR, T>::value()
{
	return _value;
}

template<typename CHAR, typename T>
const std::optional<T> &lak::trie<CHAR, T>::value() const
{
	return _value;
}

template<typename CHAR, typename T>
lak::trie<CHAR, T> *lak::trie<CHAR, T>::find(lak::string_view<CHAR> key)
{
	auto [node, remaining_key] = find(this, key);
	return remaining_key == node->_key ? node : nullptr;
}

template<typename CHAR, typename T>
const lak::trie<CHAR, T> *lak::trie<CHAR, T>::find(
  lak::string_view<CHAR> key) const
{
	auto [node, remaining_key] = find(const_cast<trie *>(this), key);
	return remaining_key == node->_key ? node : nullptr;
}

template<typename CHAR, typename T>
lak::trie<CHAR, T> &lak::trie<CHAR, T>::find_or_emplace(
  const lak::string<CHAR> &key)
{
	auto [node, remaining_key] = find(this, lak::string_view(key));

	if (remaining_key != lak::string_view(node->_key))
	{
		node->try_emplace(remaining_key.to_string());
	}

	std::tie(node, remaining_key) = find(this, lak::string_view(key));

	ASSERT_EQUAL(remaining_key, lak::string_view(node->_key));
	ASSERT(node->_value);

	return *node;
}

template<typename CHAR, typename T>
lak::trie<CHAR, T> &lak::trie<CHAR, T>::operator[](
  const lak::string<CHAR> &key)
{
	return find_or_emplace(key);
}

template<typename CHAR, typename T>
void lak::trie<CHAR, T>::try_emplace(const lak::string<CHAR> &key)
{
	if (auto node = internal_try_emplace(lak::string_view(key)); node)
		node->_value.emplace();
}

template<typename CHAR, typename T>
template<typename... ARGS>
void lak::trie<CHAR, T>::try_emplace(const lak::string<CHAR> &key,
                                     ARGS &&...args)
{
	if (auto node = internal_try_emplace(lak::string_view(key)); node)
		node->_value.emplace(lak::forward<ARGS>(args)...);
}

template<typename CHAR, typename T>
void lak::trie<CHAR, T>::force_emplace(const lak::string<CHAR> &key)
{
	internal_force_emplace(lak::string_view(key))->_value.emplace();
}

template<typename CHAR, typename T>
template<typename... ARGS>
void lak::trie<CHAR, T>::force_emplace(const lak::string<CHAR> &key,
                                       ARGS &&...args)
{
	internal_force_emplace(lak::string_view(key))
	  ->_value.emplace(lak::forward<ARGS>(args)...);
}

template<typename CHAR, typename T>
lak::trie<CHAR, T>::trie(lak::string<CHAR> &&k)
: _key(lak::move(k)), _value(std::nullopt), _map(), _nodes()
{
}

template<typename CHAR, typename T>
lak::trie<CHAR, T>::trie(lak::string<CHAR> &&k, std::optional<T> &&v)
: _key(lak::move(k)), _value(lak::move(v)), _map(), _nodes()
{
}

template<typename CHAR, typename T>
lak::trie<CHAR, T>::trie(lak::string<CHAR> &&k,
                         std::vector<char32_t> &&m,
                         std::vector<trie> &&n)
: _key(lak::move(k)),
  _value(std::nullopt),
  _map(lak::move(m)),
  _nodes(lak::move(n))
{
}

template<typename CHAR, typename T>
lak::trie<CHAR, T>::trie(lak::string<CHAR> &&k,
                         std::optional<T> &&v,
                         std::vector<char32_t> &&m,
                         std::vector<trie> &&n)
: _key(lak::move(k)),
  _value(lak::move(v)),
  _map(lak::move(m)),
  _nodes(lak::move(n))
{
}

template<typename CHAR, typename T>
lak::trie<CHAR, T> *lak::trie<CHAR, T>::internal_try_emplace(
  lak::string_view<CHAR> key)
{
	auto [node, remaining_key] = find(this, key);

	ASSERT_NOT_EQUAL(node, nullptr);

	if (remaining_key == lak::string_view(node->_key))
	{
		// exact node already exists.
		return node->_value.has_value() ? nullptr : node;
	}

	// merge a newly constructed node with the terminal node that we found.
	*node = merge(lak::move(*node), trie(remaining_key.to_string()));

	node = find(node, remaining_key).first;

	ASSERT(!node->_value.has_value());

	return node;
}

template<typename CHAR, typename T>
lak::trie<CHAR, T> *lak::trie<CHAR, T>::internal_force_emplace(
  lak::string_view<CHAR> key)
{
	auto [node, remaining_key] = find(this, key);

	if (remaining_key == lak::string_view(node->_key))
	{
		// exact node already exists.
		if (node->_value.has_value()) node->_value.reset();
		return node;
	}

	// merge a newly constructed node with the terminal node that we found.
	*node = merge(lak::move(*node), trie(remaining_key.to_string()));

	node = find(node, remaining_key).first;

	ASSERT(!node->_value.has_value());

	return node;
}

template<typename CHAR, typename T>
lak::trie<CHAR, T> lak::trie<CHAR, T>::merge(lak::trie<CHAR, T> &&node1,
                                             lak::trie<CHAR, T> &&node2)
{
	ASSERT_NOT_EQUAL(node1._key, node2._key);

	auto u32key1 = lak::strconv<char32_t, CHAR>(node1._key);
	auto u32key2 = lak::strconv<char32_t, CHAR>(node2._key);

	auto u32common =
	  lak::string_view(lak::common_initial_sequence<const char32_t>(
	    lak::string_view(u32key1), lak::string_view(u32key2)));

	const auto u32_common_length = u32common.size();
	const auto common_length     = lak::converted_string_length<CHAR>(u32common);

	if (common_length == node1._key.size())
	{
		// node1.key is a prefix of node2.key.
		const char32_t map2_key = u32key2.at(u32_common_length);
		node1._map.push_back(map2_key);
		node2._key =
		  node2._key.substr(common_length + lak::codepoint_length<CHAR>(map2_key));
		node1._nodes.emplace_back(lak::move(node2));
		return node1;
	}
	else if (common_length == node2._key.size())
	{
		// node2.key is a prefix of node1.key.
		const char32_t map1_key = u32key1.at(u32_common_length);
		node2._map.push_back(map1_key);
		node1._key =
		  node1._key.substr(common_length + lak::codepoint_length<CHAR>(map1_key));
		node2._nodes.emplace_back(lak::move(node1));
		return node2;
	}
	else
	{
		ASSERT_GREATER(node1._key.size(), common_length);
		ASSERT_GREATER(node2._key.size(), common_length);
		ASSERT_GREATER(u32key1.size(), u32_common_length);
		ASSERT_GREATER(u32key2.size(), u32_common_length);
		std::vector<char32_t> map = {u32key1.at(u32_common_length),
		                             u32key2.at(u32_common_length)};
		node1._key =
		  node1._key.substr(common_length + lak::codepoint_length<CHAR>(map[0]));
		node2._key =
		  node2._key.substr(common_length + lak::codepoint_length<CHAR>(map[1]));

		std::vector<trie> nodes;
		nodes.emplace_back(lak::move(node1));
		nodes.emplace_back(lak::move(node2));

		return trie(
		  lak::strconv<CHAR>(u32common), lak::move(map), lak::move(nodes));
	}
}

template<typename CHAR, typename T>
std::pair<lak::trie<CHAR, T> *, lak::string_view<CHAR>>
lak::trie<CHAR, T>::find(lak::trie<CHAR, T> *node, lak::string_view<CHAR> key)
{
	for (;;)
	{
		if (key.empty() || !impl::is_prefix(key, lak::string_view(node->_key)) ||
		    key == lak::string_view(node->_key))
		{
			ASSERT_NOT_EQUAL(node, nullptr);
			return {node, key};
		}

		const auto common_size = node->_key.size();
		ASSERT_GREATER(key.size(), common_size);
		const auto lookup = lak::codepoint(key.substr(common_size));

		if_in(lookup, node->_map, index)
		{
			ASSERT_GREATER(node->_nodes.size(), index);
			node = node->_nodes.data() + index;
			key  = key.substr(common_size + lak::codepoint_length<CHAR>(lookup));
		}
		else
		{
			ASSERT_NOT_EQUAL(node, nullptr);
			return {node, key};
		}
	}
}

template<typename CHAR, typename T>
lak::u8string lak::trie<CHAR, T>::to_string() const
{
	lak::u8string result;

	std::vector<lak::span<const lak::trie<CHAR, T>>> stack;

	stack.emplace_back(this, 1);

	while (stack.size() > 0)
	{
		if (stack.back().size() > 0)
		{
			auto &t = stack.back()[0];

			lak::u8string key;
			if (stack.size() > 1)
			{
				auto t_span        = stack.back();
				auto t_parent_span = stack[stack.size() - 2];
				auto &t_parent     = t_parent_span[0];
				ASSERT_GREATER_OR_EQUAL(t_parent.map().size(), t_span.size());
				size_t parent_index = t_parent.map().size() - t_span.size();
				key =
				  lak::to_u8string(lak::string_view(&t_parent.map()[parent_index], 1));
			}
			key += lak::to_u8string(t.key());

			result += lak::u8string((stack.size() - 1) * 2, ' ') + u8"\"" + key +
			          u8"\" : [{";

			if constexpr (lak::is_streamable<decltype(*t.value())>())
			{
				if (t.value())
				{
					result += lak::streamify(*t.value()) + u8"}, {";
				}
			}

			const bool has_nodes = t.nodes().size() > 0;
			if (has_nodes)
			{
				result += u8"\n";
				stack.push_back(lak::span(t.nodes()));
			}
			else
			{
				result += u8"}]";
				stack.back() = stack.back().subspan(1);
				if (stack.back().size() > 0)
				{
					result += u8",\n";
				}
			}
		}
		else
		{
			stack.pop_back();
			if (stack.size() > 0)
			{
				result += u8"\n" + lak::u8string((stack.size() - 1) * 2, ' ') + u8"}]";
				stack.back() = stack.back().subspan(1);
				if (stack.back().size() > 0)
				{
					result += u8",\n";
				}
			}
		}
	}

	return result;
}
