#pragma once

#include "reflection/details/field_entity_interface.hpp"

namespace Engine
{
    template <typename ClassType, typename FieldType>
    class FieldEntity : public IFieldEntity
    {
        using TField = FieldType(ClassType::*);
    public:
        explicit FieldEntity(TField field) : Instance(field) {}

        Variant GetValue(const Variant& obj) final
        {
            if (obj.IsValid())
            {
                if (ClassType* ptr = obj.GetPtr<ClassType>())
                {
                    return Variant(ptr->*Instance);
                }
            }

            PL_ERROR("Engine", _T("Invalid object for get property"));
            return {};
        }

        bool SetValue(const Variant& obj, const Variant& newVal) final
        {
            if (obj.IsValid() && newVal.IsValid())
            {
                if (ClassType* ptr = obj.GetPtr<ClassType>())
                {
                    ptr->*Instance = newVal.GetValue<FieldType>();
                    return true;
                }
            }

            return false;
        }

    private:
        TField Instance;
    };
}