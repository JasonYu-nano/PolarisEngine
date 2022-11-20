//#include "precompiled_core.hpp"
#include "math/hash_helper.hpp"

namespace Engine
{
    uint64 HashHelper::FnvHash(const char* str, uint64 n) noexcept
    {
        static uint64 fnvOffset = 14695981039346656037ull;
        static uint64 fnvPrime = 1099511628211ull;
        uint64 hash = fnvOffset;

        for (size_t i = 0; i < n; i++)
            hash = (hash ^ static_cast<uint64>(str[i])) * fnvPrime;

        return hash;
    }

    uint64 HashHelper::FnvHash(const StringView& str) noexcept
    {
        return FnvHash(str.Data(), str.Length());
    }
}