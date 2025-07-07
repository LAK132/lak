#include "lak/test.hpp"

#include "lak/structure/pnm.hpp"

#include "lak/binary_reader.hpp"
#include "lak/file.hpp"
#include "lak/string_literals.hpp"

BEGIN_TEST(pnm)
{
	{
		DEBUG("P7")
		auto source =
		  "P7\n"
		  "WIDTH 2\n"
		  "HEIGHT 2\n"
		  "DEPTH 1\n"
		  "MAXVAL 255\n"
		  "ENDHDR\n"
		  "\xFF\x7F\x32\x00"_span;

		lak::binary_reader strm{lak::span<const byte_t>(source)};

		auto pnm = strm.template read<lak::pnm::pnm>().UNWRAP();

		DEBUG_EXPR(pnm.width);

		auto img = (lak::image3_t)pnm;

		ASSERT_EQUAL(img.size().x, 2U);
		ASSERT_EQUAL(img.size().y, 2U);
	}

	return 0;
}
END_TEST()
