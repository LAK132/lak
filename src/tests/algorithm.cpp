#include "lak/algorithm.hpp"

struct less_test
{
	int a;
	int b;
};

static_assert(lak::less<less_test, &less_test::a, &less_test::b>{}(
  less_test{
    .a = 0,
    .b = 0,
  },
  less_test{
    .a = 1,
    .b = 1,
  }));

static_assert(lak::less<less_test, &less_test::a, &less_test::b>{}(
  less_test{
    .a = 0,
    .b = 1,
  },
  less_test{
    .a = 1,
    .b = 0,
  }));

static_assert(!lak::less<less_test, &less_test::b, &less_test::a>{}(
  less_test{
    .a = 0,
    .b = 1,
  },
  less_test{
    .a = 1,
    .b = 0,
  }));
