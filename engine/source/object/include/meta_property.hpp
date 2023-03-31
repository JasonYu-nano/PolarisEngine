#pragma once

#include "meta_attributes.hpp"

namespace Engine
{
    class OBJECT_API MetaProperty : public MetaObject
    {
    public:
        MetaProperty(StringID name, size_t offset, EMetaFlag flags)
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
}
