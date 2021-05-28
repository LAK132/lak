#include "lak/trie.hpp"

#include "lak/test.hpp"

BEGIN_TEST(trie)
{
  lak::atrie<lak::astring> trie;

  trie.try_emplace("", "first");
  trie.try_emplace("i", "second");
  DEBUG(trie);

  trie.try_emplace("j", "second2");
  DEBUG(trie);

  trie.try_emplace("int", "third");
  DEBUG(trie);

  trie.try_emplace("intint", "fourth");
  DEBUG(trie);

  trie.try_emplace("l", "fifth");
  DEBUG(trie);

  trie["long long"].value() = "seventh";
  DEBUG(trie);

  trie.try_emplace("long", "sixth");
  DEBUG(trie);

  trie.force_emplace("long", "eighth");
  DEBUG(trie);

  return 0;
}
END_TEST()