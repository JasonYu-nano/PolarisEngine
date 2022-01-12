#pragma once

#include "definitions_core.hpp"
#include "foundation/string.hpp"

namespace Engine
{
    class CORE_API FixString
    {
    public:
        FixString() = default;

        FixString(const tchar* str);

        String ToString() const;

    private:

    };
}
