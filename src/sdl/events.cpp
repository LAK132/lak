#include "lak/events.hpp"
#include "lak/debug.hpp"

bool lak::next_thread_event(const lak::platform_instance &instance,
                            lak::event *event)
{
  return false;
}

bool lak::peek_thread_event(const lak::platform_instance &instance,
                            lak::event *event)
{
  return false;
}
