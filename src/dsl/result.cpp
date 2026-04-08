#include "lak/dsl/result.hpp"

#include "lak/utility.hpp"

template struct lak::array<lak::dsl::err::parse, lak::dynamic_extent>;

template struct lak::dsl::parse_result<lak::u8string_view>;

template struct lak::result<lak::dsl::parse_result<lak::u8string_view>,
                            lak::dsl::err::parse>;
