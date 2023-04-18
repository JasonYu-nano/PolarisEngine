#pragma once

#include "meta_object.hpp"
#include "foundation/string_view.hpp"
#include "foundation/string_id.hpp"

namespace Engine
{
    class OBJECT_API MetaType
    {
    public:
        constexpr static int32 UnknownType = -1;

        template <typename T>
        static MetaType FromType();

        static MetaType FromName(StringView name);

        constexpr MetaType(int32 typeID) : TypeID(typeID) {}

        int32 ID() const { return TypeID; }

        bool operator== (const MetaType& other) const
        {
            return TypeID == other.TypeID;
        }

        bool operator!= (const MetaType& other) const
        {
            return TypeID != other.TypeID;
        }

        GMetaObject* GetMetaObject() const;

    private:
        int32 TypeID{ 0 };
        StringID TypeName;
    };

    template <typename T>
    MetaType MetaType::FromType()
    {
        return MetaType(0);
    }

    template <>
    MetaType MetaType::FromType<void>();
}
