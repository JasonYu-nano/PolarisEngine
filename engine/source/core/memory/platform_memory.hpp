#pragma once

#include "predefine/platform.hpp"

#if PLATFORM_WINDOWS
#include "memory/details/windows/windows_memory.hpp"
#else
#error "unsupport platform"
#endif