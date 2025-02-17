#include "lak/test.hpp"

#include "lak/dsl/json.hpp"
#include "lak/string_literals.hpp"

BEGIN_TEST(json)
{
	{
		auto str = u8""_view;
		auto res = lak::dsl::json.parse(str).UNWRAP().value;

		ASSERT_EQUAL(res.tokens.size(), 0U);
		ASSERT_EQUAL(res.strings.size(), 0U);
		ASSERT_EQUAL(res.numbers.size(), 0U);
		ASSERT_EQUAL(res.values.size(), 0U);
		ASSERT_EQUAL(res.arrays.size(), 0U);
		ASSERT_EQUAL(res.objects.size(), 0U);
	}
	{
		auto str = u8"{}"_view;
		auto res = lak::dsl::json.parse(str).UNWRAP().value;

		ASSERT_EQUAL(res.tokens.size(), 0U);
		ASSERT_EQUAL(res.strings.size(), 0U);
		ASSERT_EQUAL(res.numbers.size(), 0U);
		ASSERT_EQUAL(res.values.size(), 1U);
		ASSERT_EQUAL(res.arrays.size(), 0U);
		ASSERT_EQUAL(res.objects.size(), 1U);

		ASSERT_EQUAL(res.objects[0].size(), 0U);
	}
	{
		auto str = u8"[]"_view;
		auto res = lak::dsl::json.parse(str).UNWRAP().value;

		ASSERT_EQUAL(res.tokens.size(), 0U);
		ASSERT_EQUAL(res.strings.size(), 0U);
		ASSERT_EQUAL(res.numbers.size(), 0U);
		ASSERT_EQUAL(res.values.size(), 1U);
		ASSERT_EQUAL(res.arrays.size(), 1U);
		ASSERT_EQUAL(res.objects.size(), 0U);

		ASSERT_EQUAL(res.arrays[0].size(), 0U);
	}
	{
		auto str = u8"true"_view;
		auto res = lak::dsl::json.parse(str).UNWRAP().value;

		ASSERT_EQUAL(res.tokens.size(), 1U);
		ASSERT_EQUAL(res.strings.size(), 0U);
		ASSERT_EQUAL(res.numbers.size(), 0U);
		ASSERT_EQUAL(res.values.size(), 1U);
		ASSERT_EQUAL(res.arrays.size(), 0U);
		ASSERT_EQUAL(res.objects.size(), 0U);

		ASSERT_EQUAL(res.tokens[0], str);
	}
	{
		auto str = u8"false"_view;
		auto res = lak::dsl::json.parse(str).UNWRAP().value;

		ASSERT_EQUAL(res.tokens.size(), 1U);
		ASSERT_EQUAL(res.strings.size(), 0U);
		ASSERT_EQUAL(res.numbers.size(), 0U);
		ASSERT_EQUAL(res.values.size(), 1U);
		ASSERT_EQUAL(res.arrays.size(), 0U);
		ASSERT_EQUAL(res.objects.size(), 0U);

		ASSERT_EQUAL(res.tokens[0], str);
	}
	{
		auto str = u8"null"_view;
		auto res = lak::dsl::json.parse(str).UNWRAP().value;

		ASSERT_EQUAL(res.tokens.size(), 1U);
		ASSERT_EQUAL(res.strings.size(), 0U);
		ASSERT_EQUAL(res.numbers.size(), 0U);
		ASSERT_EQUAL(res.values.size(), 1U);
		ASSERT_EQUAL(res.arrays.size(), 0U);
		ASSERT_EQUAL(res.objects.size(), 0U);

		ASSERT_EQUAL(res.tokens[0], str);
	}
	{
		auto str = u8"{\"hello\":[\"world\", \"!\"], \"meow\":{0:null}}"_view;
		auto res = lak::dsl::json.parse(str).UNWRAP().value;

		ASSERT_EQUAL(res.tokens.size(), 1U);
		ASSERT_EQUAL(res.strings.size(), 4U);
		ASSERT_EQUAL(res.numbers.size(), 1U);
		ASSERT_EQUAL(res.values.size(), 9U);
		ASSERT_EQUAL(res.arrays.size(), 1U);
		ASSERT_EQUAL(res.objects.size(), 2U);
	}
	return 0;
}
END_TEST()
