#pragma once

#include "mos/meta_attributes.hpp"

namespace Engine
{
    class CORE_API MetaProperty : public MetaAttributes
    {
        using Super = MetaAttributes;
    public:
        MetaProperty(StringID name, size_t offset, EMetaFlag flags, const Map<StringID, String>& attributes)
            : Name(name)
            , Offset(offset)
            , Super(flags, attributes)
        {}

    private:
        StringID Name;
        size_t Offset{ 0 };
    };
}
