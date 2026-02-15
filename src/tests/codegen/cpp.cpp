#include "lak/test.hpp"

#include "lak/codegen/cpp.hpp"

BEGIN_TEST(codegen_cpp)
{
	lak::codegen::cpp_writer strm;

	ASSERT_EQUAL(strm.release(), u8""_view);

	strm.push_namespace(u8"lak"_view).pop_namespace();

	ASSERT_EQUAL(lak::string_view<char8_t>(strm),
	             u8"namespace lak {\n} /* lak */"_view);

	strm.write_indent_newline()
	  .push_namespace(u8"lak"_view)
	  .push_if(u8"really_cool")
	  .pop_if()
	  .pop_namespace();

	ASSERT_EQUAL(lak::string_view<char8_t>(strm),
	             u8"namespace lak {\n} /* lak */\n"
	             "namespace lak {\n"
	             "\tif (really_cool) {}\n"
	             "} /* lak */"_view);

	return EXIT_SUCCESS;
}
END_TEST();
