#pragma once

#include "reflection/meta_object.hpp"
#include "reflection/details/field_entity.hpp"

namespace Engine
{
    class Field : public MetaObject
    {
    public:
        Field() = default;

        explicit Field(IFieldEntity* entity) : Entity(entity) {}

        ~Field() override
        {
            delete Entity;
        }

        Variant GetValue(const Variant& obj) { return Entity->GetValue(obj); }

        bool SetValue(const Variant& obj, const Variant& newVal) { return Entity->SetValue(obj, newVal); }
    protected:
        IFieldEntity* Entity{ nullptr };
    };
}