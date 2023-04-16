#pragma once

#include "foundation/map.hpp"
#include "meta_types.hpp"
#include "definitions_object.hpp"
#include "object.hpp"
#include "meta_field.gen.hpp"

namespace Engine
{
    class OBJECT_API META() MetaField : public GObject
    {
        MOC_GENERATED()

        friend struct MetaConstructUtils;
    public:
        bool HasAnyFlags(EMetaFlag flags) const
        {
            return (Flags & flags) != 0;
        }

        bool HasAllFlags(EMetaFlag flags) const
        {
            return (Flags & flags) == flags;
        }

    protected:
#if WITH_META_DATA
        void SetMetaData(const std::initializer_list<Pair<StringID, String>>& data)
        {
            MetaDataMap = data;
        }
#endif

        EMetaFlag Flags{ MF_None };

#if WITH_META_DATA
        Map<StringID, String> MetaDataMap;
#endif
    };
}
