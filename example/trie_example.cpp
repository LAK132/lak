#include "lak/trie.hpp"

#include <iostream>

int main()
{
  lak::atrie<lak::astring> trie;

  trie.try_emplace("", "first");
  trie.try_emplace("i", "second");
  std::cout << trie << "\n\n";

  trie.try_emplace("j", "second2");
  std::cout << trie << "\n\n";

  trie.try_emplace("int", "third");
  std::cout << trie << "\n\n";

  trie.try_emplace("intint", "fourth");
  std::cout << trie << "\n\n";

  trie.try_emplace("l", "fifth");
  std::cout << trie << "\n\n";

  trie["long long"].value() = "seventh";
  std::cout << trie << "\n\n";

  trie.try_emplace("long", "sixth");
  std::cout << trie << "\n\n";

  trie.force_emplace("long", "eighth");
  std::cout << trie << "\n\n";

  return 0;
}

#include "../src/debug.cpp"
#include "../src/file.cpp"
#include "../src/memory.cpp"
#include "../src/strconv.cpp"
#include "../src/tinflate.cpp"
#include "../src/tokeniser.cpp"
#include "../src/unicode.cpp"
