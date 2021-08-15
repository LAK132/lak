#include "lak/events.hpp"

#include "impl.hpp"

bool lak::next_event(lak::event *e)
{
	(void)e;
	return false;
}

bool lak::peek_event(lak::event *e)
{
	(void)e;
	return false;
}

#include "../common/events.inl"
