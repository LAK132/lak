#include "lak/array.hpp"

#include "lak/test.hpp"

BEGIN_TEST(array)
{
  lak::array<int> array;
  array.reserve(0x100);
  array.resize(0x10);
  array[0] = 0x10;
  array.resize(0x1000);
  array.resize(0x100);
  DEBUG(array[0]);
  DEBUG(uintptr_t(&array[0]));
  DEBUG(array.size());
  DEBUG(array.capacity());
  DEBUG(array.reserved());

  [[maybe_unused]] auto span = lak::span(array);

  lak::array<int> array2 = lak::move(array);
  array2.resize(0x10);

  for (const auto &i : array2)
  {
    DEBUG(i);
  }

  return 0;
}
END_TEST()