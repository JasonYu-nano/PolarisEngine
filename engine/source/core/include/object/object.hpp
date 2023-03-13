#pragma once

#include "definitions_core.hpp"
#include "mos/meta_types.hpp"
#include "object.gen.hpp"

namespace Engine
{
    class CORE_API GObject
    {
        META_CLASS_GENERATED(GObject)
    public:
        static MetaClass* MetaClass() { return nullptr; }
    };
}
