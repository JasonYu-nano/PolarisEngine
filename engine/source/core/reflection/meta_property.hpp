#pragma once

#include "log/logger.hpp"
#include "reflection/meta_object.hpp"
#include "reflection/variant.hpp"
#include "log/logger.hpp"

namespace Engine
{
    class IPropertyInst
    {
    public:
        virtual ~IPropertyInst() = default;

        virtual Variant GetValue(const Variant& obj) = 0;

        virtual bool SetValue(const Variant& obj, const Variant& newVal) = 0;
    };

    template <typename ClassType, typename PropertyType>
    class PropertyInst : public IPropertyInst
    {
        using TInstance = PropertyType(ClassType::*);
    public:
        PropertyInst(TInstance inst) : Instance(inst) {}

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
            return Variant();
        }

        bool SetValue(const Variant& obj, const Variant& newVal) final
        {
            if (obj.IsValid() && newVal.IsValid())
            {
                if (ClassType* ptr = obj.GetPtr<ClassType>())
                {
                    ptr->*Instance = newVal.GetValue<PropertyType>();
                    return true;
                }
            }

            return false;
        }

    private:
        TInstance Instance;
    };

    class MetaProperty : public MetaObject
    {
    public:
        MetaProperty(IPropertyInst* inst) : Instance(inst) {}

        ~MetaProperty() override
        {
            delete Instance;
        }

        Variant GetValue(const Variant& obj) { return Instance->GetValue(obj); }

        bool SetValue(const Variant& obj, const Variant& newVal) { return Instance->SetValue(obj, newVal); }
    protected:
        IPropertyInst* Instance;
    };
}