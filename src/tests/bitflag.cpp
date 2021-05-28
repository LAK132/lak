#include "lak/bitflag.hpp"

#include "lak/test.hpp"

BEGIN_TEST(bitflag)
{
  enum struct my_enum
  {
    bit1 = 0,
    bit2 = 1,
    bit3 = 2
  };
  lak::bitflag<my_enum, my_enum::bit1, my_enum::bit3, my_enum::bit2> flags;
  flags.set<my_enum::bit1>(true);
  flags.set<my_enum::bit2>(false);
  flags.set<my_enum::bit3>(true);
  DEBUG((flags.get<my_enum::bit1>() ? "true" : "false"));
  DEBUG((flags.get<my_enum::bit2>() ? "true" : "false"));
  DEBUG((flags.get<my_enum::bit3>() ? "true" : "false"));

  return 0;
}
END_TEST()