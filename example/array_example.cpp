#include "lak/array.hpp"
#include "lak/span.hpp"

#include <iomanip>
#include <iostream>

int main()
{
  lak::array<int> array;
  array.reserve(0x100);
  array.resize(0x10);
  array[0] = 0x10;
  array.resize(0x1000);
  array.resize(0x100);
  std::cout << std::uppercase << std::hex;
  std::cout << "0x" << array[0] << "\n";
  std::cout << "0x" << uintptr_t(&array[0]) << "\n";
  std::cout << "0x" << array.size() << "\n";
  std::cout << "0x" << array.capacity() << "\n";
  std::cout << "0x" << array.reserved() << "\n";

  [[maybe_unused]] auto span = lak::span(array);

  lak::array<int> array2 = std::move(array);
  array2.resize(0x10);

  for (const auto &i : array2)
  {
    std::cout << "0x" << i << "\n";
  }

  std::cout << "\n";
}

#include "../src/debug.cpp"
#include "../src/strconv.cpp"
#include "../src/unicode.cpp"
