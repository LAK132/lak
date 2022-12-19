#include "lak/test.hpp"

#include "lak/variant.hpp"
#include "lak/visit.hpp"

BEGIN_TEST(visit)
{
	lak::variant<int, bool> var = int(0);

	auto visitor = lak::overloaded{
	  [](const int &) -> lak::optional<lak::monostate> { return lak::nullopt; },
	  [](const bool &) -> lak::optional<lak::monostate> { return lak::nullopt; },
	};

	lak::visit_switch(decltype(var)::indices{},
	                  var.index(),
	                  [&]<lak::concepts::integral_constant I>(const I &)
	                  { return visitor(*var.template get<I::value>()); });

	const int i = lak::visit(
	  lak::overloaded{
	    [](const int &i) -> int
	    {
		    (void)i;
		    return 10;
	    },
	    [](const bool &b) -> int
	    {
		    (void)b;
		    return 20;
	    },
	  },
	  var);
	ASSERT_EQUAL(i, 10);

	lak::visit(
	  lak::overloaded{
	    [](const int &i) -> lak::optional<lak::monostate>
	    {
		    (void)i;
		    return lak::optional<lak::monostate>{};
	    },
	    [](const bool &b) -> lak::optional<lak::monostate>
	    {
		    (void)b;
		    return lak::optional<lak::monostate>{};
	    },
	  },
	  var);

	lak::visit(
	  lak::overloaded{
	    [](const int &i, const int &i2) -> lak::optional<lak::monostate>
	    {
		    (void)i;
		    (void)i2;
		    return lak::optional<lak::monostate>{};
	    },
	    [](const int &i, const bool &b2) -> lak::optional<lak::monostate>
	    {
		    (void)i;
		    (void)b2;
		    return lak::optional<lak::monostate>{};
	    },
	    [](const bool &b, const int &i2) -> lak::optional<lak::monostate>
	    {
		    (void)b;
		    (void)i2;
		    return lak::optional<lak::monostate>{};
	    },
	    [](const bool &b, const bool &b2) -> lak::optional<lak::monostate>
	    {
		    (void)b;
		    (void)b2;
		    return lak::optional<lak::monostate>{};
	    },
	  },
	  var,
	  var);

	return 0;
}
END_TEST()
