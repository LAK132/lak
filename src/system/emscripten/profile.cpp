#include "lak/system/profile.hpp"
#include "lak/system/os.hpp"

#include "lak/debug.hpp"

#include "emscripten.h"

uint64_t lak::performance_frequency() { return 1'000U; }

uint64_t lak::performance_counter() { return uint64_t(emscripten_get_now()); }
