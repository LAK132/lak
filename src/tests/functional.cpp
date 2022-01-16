#include "lak/functional.hpp"

struct equality_tester
{
	signed a;
	unsigned b;
};

/* --- equal_to --- */

static_assert(
  lak::equal_to<equality_tester, &equality_tester::a, &equality_tester::b>{}(
    equality_tester{0, 0U}, equality_tester{0, 0U}));

static_assert(
  lak::equal_to<equality_tester, &equality_tester::a, &equality_tester::b>{}(
    equality_tester{1, 0U}, equality_tester{1, 0U}));

static_assert(
  lak::equal_to<equality_tester, &equality_tester::a, &equality_tester::b>{}(
    equality_tester{0, 1U}, equality_tester{0, 1U}));

static_assert(
  !lak::equal_to<equality_tester, &equality_tester::a, &equality_tester::b>{}(
    equality_tester{0, 1U}, equality_tester{1, 0U}));

static_assert(
  !lak::equal_to<equality_tester, &equality_tester::a, &equality_tester::b>{}(
    equality_tester{1, 0U}, equality_tester{0, 1U}));

/* --- not_equal_to --- */

static_assert(!lak::not_equal_to<equality_tester,
                                 &equality_tester::a,
                                 &equality_tester::b>{}(equality_tester{0, 0U},
                                                        equality_tester{0,
                                                                        0U}));

static_assert(!lak::not_equal_to<equality_tester,
                                 &equality_tester::a,
                                 &equality_tester::b>{}(equality_tester{1, 0U},
                                                        equality_tester{1,
                                                                        0U}));

static_assert(!lak::not_equal_to<equality_tester,
                                 &equality_tester::a,
                                 &equality_tester::b>{}(equality_tester{0, 1U},
                                                        equality_tester{0,
                                                                        1U}));

static_assert(lak::not_equal_to<equality_tester,
                                &equality_tester::a,
                                &equality_tester::b>{}(equality_tester{0, 1U},
                                                       equality_tester{1,
                                                                       0U}));

static_assert(lak::not_equal_to<equality_tester,
                                &equality_tester::a,
                                &equality_tester::b>{}(equality_tester{1, 0U},
                                                       equality_tester{0,
                                                                       1U}));

/* --- less --- */

static_assert(
  !lak::less<equality_tester, &equality_tester::a, &equality_tester::b>{}(
    equality_tester{0, 0U}, equality_tester{0, 0U}));

static_assert(
  !lak::less<equality_tester, &equality_tester::a, &equality_tester::b>{}(
    equality_tester{1, 0U}, equality_tester{1, 0U}));

static_assert(
  !lak::less<equality_tester, &equality_tester::a, &equality_tester::b>{}(
    equality_tester{0, 1U}, equality_tester{0, 1U}));

static_assert(
  lak::less<equality_tester, &equality_tester::a, &equality_tester::b>{}(
    equality_tester{0, 1U}, equality_tester{1, 0U}));

static_assert(
  !lak::less<equality_tester, &equality_tester::a, &equality_tester::b>{}(
    equality_tester{1, 0U}, equality_tester{0, 1U}));

/* --- greater --- */

static_assert(
  !lak::greater<equality_tester, &equality_tester::a, &equality_tester::b>{}(
    equality_tester{0, 0U}, equality_tester{0, 0U}));

static_assert(
  !lak::greater<equality_tester, &equality_tester::a, &equality_tester::b>{}(
    equality_tester{1, 0U}, equality_tester{1, 0U}));

static_assert(
  !lak::greater<equality_tester, &equality_tester::a, &equality_tester::b>{}(
    equality_tester{0, 1U}, equality_tester{0, 1U}));

static_assert(
  !lak::greater<equality_tester, &equality_tester::a, &equality_tester::b>{}(
    equality_tester{0, 1U}, equality_tester{1, 0U}));

static_assert(
  lak::greater<equality_tester, &equality_tester::a, &equality_tester::b>{}(
    equality_tester{1, 0U}, equality_tester{0, 1U}));

/* --- less_equal --- */

static_assert(
  lak::less_equal<equality_tester, &equality_tester::a, &equality_tester::b>{}(
    equality_tester{0, 0U}, equality_tester{0, 0U}));

static_assert(
  lak::less_equal<equality_tester, &equality_tester::a, &equality_tester::b>{}(
    equality_tester{1, 0U}, equality_tester{1, 0U}));

static_assert(
  lak::less_equal<equality_tester, &equality_tester::a, &equality_tester::b>{}(
    equality_tester{0, 1U}, equality_tester{0, 1U}));

static_assert(
  lak::less_equal<equality_tester, &equality_tester::a, &equality_tester::b>{}(
    equality_tester{0, 1U}, equality_tester{1, 0U}));

static_assert(!lak::less_equal<equality_tester,
                               &equality_tester::a,
                               &equality_tester::b>{}(equality_tester{1, 0U},
                                                      equality_tester{0, 1U}));

/* --- greater_equal --- */

static_assert(lak::greater_equal<equality_tester,
                                 &equality_tester::a,
                                 &equality_tester::b>{}(equality_tester{0, 0U},
                                                        equality_tester{0,
                                                                        0U}));

static_assert(lak::greater_equal<equality_tester,
                                 &equality_tester::a,
                                 &equality_tester::b>{}(equality_tester{1, 0U},
                                                        equality_tester{1,
                                                                        0U}));

static_assert(lak::greater_equal<equality_tester,
                                 &equality_tester::a,
                                 &equality_tester::b>{}(equality_tester{0, 1U},
                                                        equality_tester{0,
                                                                        1U}));

static_assert(!lak::greater_equal<equality_tester,
                                  &equality_tester::a,
                                  &equality_tester::b>{}(
  equality_tester{0, 1U}, equality_tester{1, 0U}));

static_assert(lak::greater_equal<equality_tester,
                                 &equality_tester::a,
                                 &equality_tester::b>{}(equality_tester{1, 0U},
                                                        equality_tester{0,
                                                                        1U}));
