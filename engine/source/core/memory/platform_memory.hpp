#pragma once

#include "global.hpp"

#if PLATFORM_WINDOWS
#include "memory/details/windows/windows_memory.hpp"
#else
#error "unsupport platform"
#endif