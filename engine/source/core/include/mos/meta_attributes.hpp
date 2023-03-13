#pragma once

#include "global.hpp"
#include "foundation/map.hpp"
#include "foundation/string_id.hpp"
#include "mos/meta_types.hpp"

namespace Engine
{
    class CORE_API MetaAttributes
    {
    public:
        MetaAttributes(EMetaFlag flags, const Map<StringID, String>& attributes)
            : Flags(flags)
            , Attributes(attributes)
        {}

        bool HasAnyFlags(EMetaFlag flags) const
        {
            return (Flags & flags) != 0;
        }

        bool HasAllFlags(EMetaFlag flags) const
        {
            return (Flags & flags) == flags;
        }

    protected:
        EMetaFlag Flags;
        Map<StringID, String> Attributes;
    };
}
