#include "lak/test.hpp"

#include "lak/file/json.hpp"

#include "lak/string_literals/view.hpp"

BEGIN_TEST(json)
{
	{
		auto str = u8""_view;
		auto res = lak::dsl::json.parse(str).UNWRAP().value;

		ASSERT_EQUAL(res.tokens().size(), 0U);
		ASSERT_EQUAL(res.strings().size(), 0U);
		ASSERT_EQUAL(res.numbers().size(), 0U);
		ASSERT_EQUAL(res.values().size(), 0U);
		ASSERT_EQUAL(res.arrays().size(), 0U);
		ASSERT_EQUAL(res.objects().size(), 0U);
	}
	{
		auto str = u8"{}"_view;
		auto res = lak::dsl::json.parse(str).UNWRAP().value;

		ASSERT_EQUAL(res.tokens().size(), 0U);
		ASSERT_EQUAL(res.strings().size(), 0U);
		ASSERT_EQUAL(res.numbers().size(), 0U);
		ASSERT_EQUAL(res.values().size(), 1U);
		ASSERT_EQUAL(res.arrays().size(), 0U);
		ASSERT_EQUAL(res.objects().size(), 1U);

		ASSERT_EQUAL(res.objects()[0].values.size, 0U);
	}
	{
		auto str = u8"[]"_view;
		auto res = lak::dsl::json.parse(str).UNWRAP().value;

		ASSERT_EQUAL(res.tokens().size(), 0U);
		ASSERT_EQUAL(res.strings().size(), 0U);
		ASSERT_EQUAL(res.numbers().size(), 0U);
		ASSERT_EQUAL(res.values().size(), 1U);
		ASSERT_EQUAL(res.arrays().size(), 1U);
		ASSERT_EQUAL(res.objects().size(), 0U);

		ASSERT_EQUAL(res.arrays()[0].values.size, 0U);
	}
	{
		auto str = u8"true"_view;
		auto res = lak::dsl::json.parse(str).UNWRAP().value;

		ASSERT_EQUAL(res.tokens().size(), 1U);
		ASSERT_EQUAL(res.strings().size(), 0U);
		ASSERT_EQUAL(res.numbers().size(), 0U);
		ASSERT_EQUAL(res.values().size(), 1U);
		ASSERT_EQUAL(res.arrays().size(), 0U);
		ASSERT_EQUAL(res.objects().size(), 0U);

		ASSERT_EQUAL(res.tokens()[0].value, str);
	}
	{
		auto str = u8"false"_view;
		auto res = lak::dsl::json.parse(str).UNWRAP().value;

		ASSERT_EQUAL(res.tokens().size(), 1U);
		ASSERT_EQUAL(res.strings().size(), 0U);
		ASSERT_EQUAL(res.numbers().size(), 0U);
		ASSERT_EQUAL(res.values().size(), 1U);
		ASSERT_EQUAL(res.arrays().size(), 0U);
		ASSERT_EQUAL(res.objects().size(), 0U);

		ASSERT_EQUAL(res.tokens()[0].value, str);
	}
	{
		auto str = u8"null"_view;
		auto res = lak::dsl::json.parse(str).UNWRAP().value;

		ASSERT_EQUAL(res.tokens().size(), 1U);
		ASSERT_EQUAL(res.strings().size(), 0U);
		ASSERT_EQUAL(res.numbers().size(), 0U);
		ASSERT_EQUAL(res.values().size(), 1U);
		ASSERT_EQUAL(res.arrays().size(), 0U);
		ASSERT_EQUAL(res.objects().size(), 0U);

		ASSERT_EQUAL(res.tokens()[0].value, str);
	}
	{
		auto str          = u8"\"my \\u0008 string \\r\\n\""_view;
		auto raw_expected = u8"my \\u0008 string \\r\\n"_view;
		auto expected     = u8"my "_str + char8_t(0x08) + u8" string \r\n"_str;
		auto res          = lak::dsl::json.parse(str).UNWRAP().value;

		ASSERT_EQUAL(res.tokens().size(), 0U);
		ASSERT_EQUAL(res.strings().size(), 1U);
		ASSERT_EQUAL(res.numbers().size(), 0U);
		ASSERT_EQUAL(res.values().size(), 1U);
		ASSERT_EQUAL(res.arrays().size(), 0U);
		ASSERT_EQUAL(res.objects().size(), 0U);

		res.intern();

		ASSERT_EQUAL(res.strings()[0].value, raw_expected);
		ASSERT_EQUAL(res.root().string().UNWRAP(), expected);
	}
	{
		auto str = u8"{\"hello\":[\"world\", \"!\"], \"meow\":{0:null}}"_view;
		auto res = lak::dsl::json.parse(str).UNWRAP().value;

		ASSERT_EQUAL(res.tokens().size(), 1U);
		ASSERT_EQUAL(res.strings().size(), 4U);
		ASSERT_EQUAL(res.numbers().size(), 1U);
		ASSERT_EQUAL(res.values().size(), 9U);
		ASSERT_EQUAL(res.arrays().size(), 1U);
		ASSERT_EQUAL(res.objects().size(), 2U);

		res.intern();

		ASSERT_EQUAL(res.objects()[0].size(), 1U);
		ASSERT_EQUAL(res.objects()[1].size(), 2U);

		auto root = res.root().object().UNWRAP();
		auto arr  = root[u8"hello"_view]->array().UNWRAP();
		ASSERT_EQUAL(arr[0U].string().UNWRAP(), u8"world"_view);
		ASSERT_EQUAL(arr[1U].string().UNWRAP(), u8"!"_view);
		auto obj = root[u8"meow"_view]->object().UNWRAP();
		auto tok = obj[u8"0"_view]->token().UNWRAP();
		ASSERT_EQUAL(tok, u8"null"_view);
	}
	return 0;
}
END_TEST()
