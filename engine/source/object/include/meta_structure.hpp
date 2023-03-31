#pragma once

#include "global.hpp"
#include "log/logger.hpp"
#include "foundation/string_id.hpp"
#include "foundation/map.hpp"
#include "meta_attributes.hpp"
#include "meta_property.hpp"
#include "meta_method.hpp"

namespace Engine
{
    class OBJECT_API MetaClass : public MetaObject
    {
        friend struct MetaConstructUtils;
    public:
        MetaClass(StringID name, MetaClass* super, EMetaFlag flags)
            : SuperClass(super)
            , ClassName(name)
        {
            Flags = SuperClass ? static_cast<EMetaFlag>(flags | SuperClass->Flags) : flags;
        }

        bool FindProperty(StringID name, MetaProperty* property)
        {
            int32 index = Properties.Find([&name](const MetaProperty& elem){
                return elem.GetPropertyName() == name;
            });

            if (index == INDEX_NONE)
            {
                property = nullptr;
                return false;
            }

            property = &Properties[index];
            return true;
        }

        bool FindMethod(StringID name, MetaMethod* method)
        {
            method = Methods.Find(name);
            return method != nullptr;
        }

    private:
        MetaProperty& AddProperty(MetaProperty&& property)
        {
            Properties.Add(std::forward<MetaProperty>(property));
            return Properties.Last();
        }

        MetaMethod& AddMethod(MetaMethod&& method)
        {
            return Methods.Add(method.GetMethodName(), std::forward<MetaMethod>(method));
        }
    private:
        MetaClass* SuperClass{ nullptr };
        StringID ClassName;
        Array<MetaProperty> Properties;
        Map<StringID, MetaMethod> Methods;
    };

    class OBJECT_API MetaStruct : public MetaObject
    {
        friend struct MetaConstructUtils;
    public:
        MetaStruct(StringID name, MetaStruct* super, EMetaFlag flags)
            : SuperStruct(super)
            , StructName(name)
        {
            Flags = SuperStruct ? static_cast<EMetaFlag>(flags | SuperStruct->Flags) : flags;
        }

        bool FindProperty(StringID name, MetaProperty* property)
        {
            int32 index = Properties.Find([&name](const MetaProperty& elem){
                return elem.GetPropertyName() == name;
            });

            if (index == INDEX_NONE)
            {
                property = nullptr;
                return false;
            }

            property = &Properties[index];
            return true;
        }

    private:
        MetaProperty& AddProperty(MetaProperty&& property)
        {
            Properties.Add(std::forward<MetaProperty>(property));
            return Properties.Last();
        }

    private:
        MetaStruct* SuperStruct;
        StringID StructName;
        Array<MetaProperty> Properties;
    };

    template <typename T>
    MetaClass* GetClass()
    {
        static_assert(FalseExpr<T>, "Can't match MetaClass");
        return nullptr;
    }

    template <typename T>
    MetaStruct* GetStruct()
    {
        static_assert(FalseExpr<T>, "Can't match MetaStruct");
        return nullptr;
    }
}
