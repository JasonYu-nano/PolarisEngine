#pragma once

#include "definitions_core.hpp"

namespace Engine
{
    class CORE_API Variant
    {
    public:

        template <typename T>
        const T& GetValue() const;

        template <typename T>
        T& GetValue();
    };
}
