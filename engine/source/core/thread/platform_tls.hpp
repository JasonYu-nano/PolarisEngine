#pragma once

#include "predefine/platform.hpp"

#if PLATFORM_WINDOWS
#include "thread/details/windows/windows_tls.hpp"
#else
#error "unsupport platform"
#endif