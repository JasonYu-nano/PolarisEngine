#pragma once

#include "reflection/meta_property.hpp"
#include "foundation/dynamic_array.hpp"

namespace Engine
{
    class CORE_API SetType
    {
    protected:
        DynamicArray<MetaProperty*> Properties;
    };
}