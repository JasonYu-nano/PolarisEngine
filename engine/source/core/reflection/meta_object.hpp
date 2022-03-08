#pragma once

#include "definitions_core.hpp"
#include "predefine/platform.hpp"
#include "foundation/fixed_string.hpp"

namespace Engine
{
    class MetaObject
    {
    public:
        virtual ~MetaObject() = default;

        FixedString GetName() const
        {
            return Name;
        }

    protected:
        FixedString Name{ FixedString::None() };
    };
}