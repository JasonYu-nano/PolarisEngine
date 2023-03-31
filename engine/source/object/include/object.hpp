#pragma once

#include "definitions_object.hpp"
#include "meta_types.hpp"
#include "foundation/string_id.hpp"
#include "object.gen.hpp"

namespace Engine
{
    class OBJECT_API META() GObject
    {
        META_CLASS_GENERATED(GObject)

    protected:
        META()
        StringID Name;
    };
}
