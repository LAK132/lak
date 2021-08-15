#include "lak/events.hpp"

template struct lak::unique_ptr<lak::platform_event>;

const lak::platform_event &lak::event::platform() const
{
	if (!_platform_event) _platform_event = lak::platform_event_ptr::make();
	return *_platform_event;
}

lak::platform_event &lak::event::platform()
{
	if (!_platform_event) _platform_event = lak::platform_event_ptr::make();
	return *_platform_event;
}

lak::event::event(lak::event_type t,
                  lak::platform_event_ptr &&e,
                  const state_t &s)
: type(t), handle(nullptr), _platform_event(lak::move(e)), _state(s)
{
}

lak::event::event(lak::event_type t, lak::platform_event_ptr &&e)
: type(t), handle(nullptr), _platform_event(lak::move(e)), _state()
{
}

lak::event::event(lak::event_type t,
                  const lak::window_handle *w,
                  lak::platform_event_ptr &&e,
                  const state_t &s)
: type(t), handle(w), _platform_event(lak::move(e)), _state(s)
{
}

lak::event::event(lak::event_type t,
                  const lak::window_handle *w,
                  lak::platform_event_ptr &&e)
: type(t), handle(w), _platform_event(lak::move(e)), _state()
{
}

lak::event::~event() = default;
