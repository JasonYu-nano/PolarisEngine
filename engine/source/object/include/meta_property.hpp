#pragma once

#include "meta_field.hpp"
#include "meta_property.gen.hpp"

namespace Engine
{
    class OBJECT_API META() GMetaProperty : public GMetaField
    {
        MOC_GENERATED()
    public:
        GMetaProperty(StringID name, size_t offset, EMetaFlag flags)
            : Name(name)
            , Offset(offset)
        {
            Flags = flags;
        }

        StringID GetPropertyName() const { return Name; }

    private:
        StringID Name;
        size_t Offset{ 0 };
    };

    class OBJECT_API META() GBoolProperty : public GMetaProperty
    {
        MOC_GENERATED()
    public:
    };
}
