#include "lak/com_ptr.hpp"

#include "lak/test.hpp"

struct com_ptr_test_type
{
	int *value;
};

template<>
struct lak::unique_com_ptr_traits<com_ptr_test_type>
{
	using handle_type  = com_ptr_test_type *;
	using exposed_type = com_ptr_test_type *;

	inline static lak::infallible_result<exposed_type> ctor(int *value)
	{
		*value = 1;
		return lak::ok_t{new com_ptr_test_type{value}};
	}

	inline static void dtor(handle_type handle)
	{
		*(handle->value) = 2;
		delete handle;
	}
};

BEGIN_TEST(unique_com_ptr)
{
	int value = 0;
	{
		auto ptr{lak::unique_com_ptr<com_ptr_test_type>::create(&value)};
		ASSERT_EQUAL(value, 1);
	}
	ASSERT_EQUAL(value, 2);

	return 0;
}
END_TEST()

template<>
struct lak::shared_com_ptr_traits<com_ptr_test_type>
{
	using handle_type  = com_ptr_test_type *;
	using exposed_type = com_ptr_test_type *;

	inline static lak::infallible_result<exposed_type> ctor(int *value)
	{
		*value = 1;
		return lak::ok_t{new com_ptr_test_type{value}};
	}

	inline static handle_type ref(handle_type handle)
	{
		++*handle->value;
		return handle;
	}

	inline static void unref(handle_type handle)
	{
		--*handle->value;
		if (*handle->value == 0) delete handle;
	}
};

BEGIN_TEST(shared_com_ptr)
{
	int value = 0;
	{
		auto ptr{lak::shared_com_ptr<com_ptr_test_type>::create(&value)};
		ASSERT_EQUAL(value, 1);
	}
	ASSERT_EQUAL(value, 0);

	{
		auto ptr1{lak::shared_com_ptr<com_ptr_test_type>::create(&value)};
		ASSERT_EQUAL(value, 1);
		{
			auto ptr2{ptr1};
			ASSERT_EQUAL(value, 2);
		}
		ASSERT_EQUAL(value, 1);
	}
	ASSERT_EQUAL(value, 0);

	return 0;
}
END_TEST()

struct com_ptr_ref_count_test
{
	int *value = nullptr;

	com_ptr_ref_count_test(int *p) : value(p) {}

	com_ptr_ref_count_test()                                          = default;
	com_ptr_ref_count_test(com_ptr_ref_count_test &&)                 = default;
	com_ptr_ref_count_test &operator=(com_ptr_ref_count_test &&)      = default;
	com_ptr_ref_count_test(const com_ptr_ref_count_test &)            = delete;
	com_ptr_ref_count_test &operator=(const com_ptr_ref_count_test &) = delete;
};

template<>
struct lak::unique_com_ptr_traits<com_ptr_ref_count_test>
{
	using handle_type  = com_ptr_ref_count_test *;
	using exposed_type = com_ptr_ref_count_test *;

	inline static lak::infallible_result<exposed_type> ctor(int *value)
	{
		++*value;
		com_ptr_ref_count_test v(value);
		return lak::ok_t{new com_ptr_ref_count_test(lak::move(v))};
	}

	inline static void dtor(handle_type handle)
	{
		--*(handle->value);
		delete handle;
	}
};

template<>
struct lak::shared_com_ptr_traits<com_ptr_ref_count_test>
: public lak::ref_count_unique_com_ptr_traits_impl<com_ptr_ref_count_test>
{
};

BEGIN_TEST(shared_unique_com_ptr)
{
	int value = 0;
	{
		auto ptr{lak::shared_com_ptr<com_ptr_ref_count_test>::create(&value)};
		ASSERT_EQUAL(value, 1);
	}
	ASSERT_EQUAL(value, 0);

	{
		auto ptr1{lak::shared_com_ptr<com_ptr_ref_count_test>::create(&value)};
		ASSERT_EQUAL(value, 1);
		{
			auto ptr2{ptr1};
			ASSERT_EQUAL(value, 1);
		}
		ASSERT_EQUAL(value, 1);
	}
	ASSERT_EQUAL(value, 0);

	return 0;
}
END_TEST()
