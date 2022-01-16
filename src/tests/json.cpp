#include "lak/json.hpp"
#include "lak/debug.hpp"
#include "lak/math.hpp"
#include "lak/string_literals.hpp"

#include "lak/test.hpp"

using lak::JSON;

BEGIN_TEST(json_empty)
{
	JSON::parse(u8R"()"_view).EXPECT_ERR();
	return 0;
}
END_TEST()

BEGIN_TEST(json_empty_object)
{
	JSON result = JSON::parse(u8R"({})"_view).UNWRAP();

	ASSERT(result.value().UNWRAP().get_object().UNWRAP().empty());

	return 0;
}
END_TEST()

BEGIN_TEST(json_object)
{
	JSON result = JSON::parse(u8R"({"a":"hello", "b": -5.0E-1})"_view).UNWRAP();

	size_t loop_count = 0;

	result.value().UNWRAP().get_object().UNWRAP().for_each(
	  [&](lak::string_view<char8_t> k, JSON::value_proxy v)
	  {
		  if (loop_count == 0)
		  {
			  ASSERT_EQUAL(k, u8"a"_view);
			  ASSERT_EQUAL(v.get_string().UNWRAP(), u8"hello"_view);
		  }
		  if (loop_count == 1)
		  {
			  ASSERT_EQUAL(k, u8"b"_view);
			  ASSERT_EQUAL(v.get_number().UNWRAP().floating_point().UNWRAP(), -0.5);
		  }
		  ++loop_count;
	  });

	ASSERT_EQUAL(loop_count, 2U);

	return 0;
}
END_TEST()

BEGIN_TEST(json_empty_array)
{
	JSON result = JSON::parse(u8R"([])"_view).UNWRAP();

	ASSERT(result.value().UNWRAP().get_array().UNWRAP().empty());

	return 0;
}
END_TEST()

BEGIN_TEST(json_array)
{
	JSON result = JSON::parse(u8R"([0, 1, 2, 3])"_view).UNWRAP();

	size_t loop_count = 0;

	result.value().UNWRAP().get_array().UNWRAP().for_each(
	  [&](JSON::value_proxy v)
	  {
		  ASSERT_EQUAL(v.get_number().UNWRAP().unsigned_integer().UNWRAP(),
		               loop_count);
		  ++loop_count;
	  });

	ASSERT_EQUAL(loop_count, 4U);

	return 0;
}
END_TEST()

BEGIN_TEST(json_empty_string)
{
	JSON result = JSON::parse(u8R"("")"_view).UNWRAP();

	ASSERT_EQUAL(result.value().UNWRAP().get_string().UNWRAP(), u8""_view);

	return 0;
}
END_TEST()

BEGIN_TEST(json_string)
{
	JSON result = JSON::parse(u8R"("hello, world!")"_view).UNWRAP();

	ASSERT_EQUAL(result.value().UNWRAP().get_string().UNWRAP(),
	             u8"hello, world!"_view);

	return 0;
}
END_TEST()

BEGIN_TEST(json_escape_string)
{
	JSON result =
	  JSON::parse(u8R"("\" \\ \/ \b\f\n\r\t\u0000\u0020\uD83D\uDC08")"_view)
	    .UNWRAP();

	char8_t expected[] = {
	  u8'"',
	  u8' ',
	  u8'\\',
	  u8' ',
	  u8'/',
	  u8' ',
	  u8'\x08',
	  u8'\x0C',
	  u8'\x0A',
	  u8'\x0D',
	  u8'\x09',
	  u8'\x00',
	  u8' ',
	  char8_t(0xF0),
	  char8_t(0x9F),
	  char8_t(0x90),
	  char8_t(0x88),
	};

	ASSERT_ARRAY_EQUAL(result.value().UNWRAP().get_string().UNWRAP(),
	                   lak::span(expected));

	return 0;
}
END_TEST()

BEGIN_TEST(json_unsigned_number)
{
	JSON result = JSON::parse(u8R"(100)"_view).UNWRAP();

	ASSERT_EQUAL(
	  result.value().UNWRAP().get_number().UNWRAP().unsigned_integer().UNWRAP(),
	  100U);

	return 0;
}
END_TEST()

BEGIN_TEST(json_signed_number)
{
	JSON result = JSON::parse(u8R"(-100)"_view).UNWRAP();

	ASSERT_EQUAL(
	  result.value().UNWRAP().get_number().UNWRAP().signed_integer().UNWRAP(),
	  -100);

	return 0;
}
END_TEST()

BEGIN_TEST(json_float_number)
{
	JSON result = JSON::parse(u8R"(-1.005e2)"_view).UNWRAP();

	ASSERT(lak::close_to(
	  result.value().UNWRAP().get_number().UNWRAP().floating_point().UNWRAP(),
	  -100.5));

	return 0;
}
END_TEST()

BEGIN_TEST(json_complex)
{
	JSON result = JSON::parse(
	                u8R"({
  "a":"hello\u0020\u000A\t",
  "b"   :
   -5.0E-1,
  "c": [
    {},
    [],
    {
      "uint": 100,
      "int": -100,
      "float": 100.0
    }
  ]
})"_view)
	                .UNWRAP();

	size_t loop_count = 0;

	result.value().UNWRAP().get_object().UNWRAP().for_each(
	  [&](lak::string_view<char8_t> k, JSON::value_proxy v)
	  {
		  if (loop_count == 0)
		  {
			  ASSERT_EQUAL(k, u8"a"_view);
			  ASSERT_EQUAL(v.get_string().UNWRAP(), u8"hello \n\t"_view);
		  }
		  if (loop_count == 1)
		  {
			  ASSERT_EQUAL(k, u8"b"_view);
			  ASSERT_EQUAL(v.get_number().UNWRAP().floating_point().UNWRAP(), -0.5);
		  }
		  if (loop_count == 3)
		  {
			  ASSERT_EQUAL(k, u8"c"_view);
			  auto arr = v.get_array().UNWRAP();
			  ASSERT(arr[0].UNWRAP().get_object().UNWRAP().empty());
			  ASSERT(arr[1].UNWRAP().get_array().UNWRAP().empty());
			  auto obj = arr[2].UNWRAP().get_object().UNWRAP();
			  ASSERT_EQUAL(obj[u8"uint"_view]
			                 .UNWRAP()
			                 .get_number()
			                 .UNWRAP()
			                 .unsigned_integer()
			                 .UNWRAP(),
			               100U);
			  ASSERT_EQUAL(obj[u8"int"_view]
			                 .UNWRAP()
			                 .get_number()
			                 .UNWRAP()
			                 .signed_integer()
			                 .UNWRAP(),
			               -100);
			  ASSERT_EQUAL(obj[u8"float"_view]
			                 .UNWRAP()
			                 .get_number()
			                 .UNWRAP()
			                 .floating_point()
			                 .UNWRAP(),
			               100.0);
		  }
		  ++loop_count;
	  });

	ASSERT_EQUAL(loop_count, 3U);

	return 0;
}
END_TEST()
