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

	const int i = lak::visit(var,
	                         lak::overloaded{
	                           [](const int &i) -> int
	                           {
		                           (void)i;
		                           return 10;
	                           },
	                           [](const bool &b) -> int
	                           {
		                           (void)b;
		                           return 10;
	                           },
	                         });
	ASSERT_EQUAL(i, 10);

	lak::visit(var,
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
	           });

	return 0;
}
END_TEST()
