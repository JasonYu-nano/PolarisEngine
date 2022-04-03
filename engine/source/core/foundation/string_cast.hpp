#pragma once

#include "predefine/platform.hpp"
#include "definitions_core.hpp"

namespace Engine
{
    CORE_API const wchar* A2W(const ansi* original);

    CORE_API const ansi* W2A(const wchar* original);
}
