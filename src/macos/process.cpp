#include "lak/process.hpp"
#include "lak/memmanip.hpp"

struct lak::process_impl
{
};

template struct lak::unique_ptr<lak::process_impl>;

lak::process::process(lak::unique_ptr<lak::process_impl> &&impl)
: _impl(lak::move(impl))
{
}

lak::result<lak::process> lak::process::create(
  const lak::fs::path &app, lak::string_view<char8_t> arguments)
{
	return lak::err_t{};
}

lak::process::~process()
{
	release();
}

lak::process::operator bool() const
{
	return false;
}

lak::result<int> lak::process::join()
{
	return lak::err_t{};
}

void lak::process::release()
{
	_impl.reset();
}

std::ostream *lak::process::std_in() const
{
	return nullptr;
}

std::istream *lak::process::std_out() const
{
	return nullptr;
}

std::istream *lak::process::std_err() const
{
	return nullptr;
}
