#pragma once
#include "definitions_core.hpp"
#include "foundation/string_view.hpp"

namespace Engine
{
    class CORE_API HashHelper
    {
    public:
        static uint64 FnvHash(const char* str, uint64 n) noexcept;

        static uint64 FnvHash(const StringView& str) noexcept;
    };
}
