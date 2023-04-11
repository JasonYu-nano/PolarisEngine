#pragma once

#include "meta_type.hpp"

namespace Engine
{
    class OBJECT_API Variant
    {
    public:

        template <typename T>
        const T& GetValue() const;

        template <typename T>
        T& GetValue();

    private:
        bool CanConvertTo(MetaType metaType) const;

    private:
        void* Data{ nullptr };
        int32 TypeID{ 0 };
    };

    template <typename T>
    T& Variant::GetValue()
    {
        MetaType metaType = MetaType::FromType<T>();
        ENSURE(metaType != MetaType::UnknownType);

        if (metaType.ID() == TypeID)
        {
            return static_cast<T>(Data);
        }

        ENSURE(CanConvertTo(metaType));
        return static_cast<T>(Data);
    }

    template <typename T>
    const T& Variant::GetValue() const
    {
        MetaType metaType = MetaType::FromType<T>();
        ENSURE(metaType != MetaType::UnknownType);

        if (metaType.ID() == TypeID)
        {
            return static_cast<T>(Data);
        }

        ENSURE(CanConvertTo(metaType));
        return static_cast<const T>(Data);
    }
}
