#include "lak/dsl/result.hpp"

lak::dsl::err::multi::multi() = default;

lak::dsl::err::multi::multi(multi &&) = default;

lak::dsl::err::multi::multi(const multi &) = default;

lak::dsl::err::multi &lak::dsl::err::multi::operator=(multi &&) = default;

lak::dsl::err::multi &lak::dsl::err::multi::operator=(const multi &) = default;

lak::dsl::err::multi::multi(lak::array<lak::dsl::err::parse> &&errs)
: errors(lak::move(errs))
{
}
