#pragma once

#include "reflection/variant.hpp"

namespace Engine
{
    class IFieldEntity
    {
    public:
        virtual ~IFieldEntity() = default;

        virtual Variant GetValue(const Variant& obj) = 0;

        virtual bool SetValue(const Variant& obj, const Variant& newVal) = 0;
    };
}