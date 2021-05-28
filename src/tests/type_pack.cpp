#include "lak/type_pack.hpp"

#include "lak/test.hpp"

#define CHECK_SAME(...)                                                       \
  static_assert(std::is_same_v<__VA_ARGS__>, "Types not the same")

// empty type_pack
CHECK_SAME(lak::type_pack<>, lak::type_pack<>);

// simple type_pack
CHECK_SAME(lak::type_pack<int>, lak::type_pack<int>);

// prepend type to beginning of type_pack
CHECK_SAME(lak::type_pack<long, int>,
           lak::prepend_to_pack_t<long, lak::type_pack<int>>);

// append type to end of type_pack
CHECK_SAME(lak::type_pack<long, int>,
           lak::append_to_pack_t<lak::type_pack<long>, int>);

// concatenate type_packs
CHECK_SAME(lak::type_pack<long, int>,
           lak::concat_packs_t<lak::type_pack<long>, lak::type_pack<int>>);

// remove type from type_pack
CHECK_SAME(lak::type_pack<long, int>,
           lak::remove_from_pack_t<double, lak::type_pack<double, long, int>>);
CHECK_SAME(lak::type_pack<long, int>,
           lak::remove_from_pack_t<double, lak::type_pack<long, double, int>>);
CHECK_SAME(lak::type_pack<long, int>,
           lak::remove_from_pack_t<double, lak::type_pack<long, int, double>>);

// remove all duplicate types from type_pack
CHECK_SAME(
  lak::type_pack<long, int, double>,
  lak::unique_pack_t<lak::type_pack<long, int, long, double, double>>);

// create a type_pack from a function
CHECK_SAME(lak::type_pack<long, int>, lak::pack_from_function_t<long(int)>);
CHECK_SAME(lak::type_pack<long, int>,
           lak::pack_from_function_t<long (*)(int)>);
CHECK_SAME(lak::type_pack<long, int>,
           lak::pack_from_function_t<long (**)(int)>);

// create a test_type with the template types from a type_pack
template<typename... T>
struct test_type
{
};
CHECK_SAME(
  test_type<int, long, double>,
  lak::create_from_pack_t<test_type, lak::type_pack<int, long, double>>);

BEGIN_TEST(type_pack)
{
  return 0;
}
END_TEST()