#include "lak/alloc.hpp"

#include "lak/result.hpp"
#include "lak/test.hpp"

BEGIN_TEST(local_alloc)
{
	// auto span{lak::local_alloc(10).UNWRAP()};

	// lak::local_free(span);

	return 0;
}
END_TEST()

BEGIN_TEST(global_alloc)
{
	auto span{lak::global_alloc(1000U).UNWRAP()};

	ASSERT_NOT_EQUAL(span.data(), nullptr);
	ASSERT_EQUAL(span.size(), 1000U);

	uint8_t i = 0;
	for (byte_t &b : span) b = byte_t(i);

	i = 0;
	for (byte_t &b : span) ASSERT_EQUAL(uint8_t(b), i);

	lak::global_free(span);

	return 0;
}
END_TEST()
