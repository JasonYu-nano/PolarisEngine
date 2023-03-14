#pragma once

#include "global.hpp"
#include "log/logger.hpp"
#include "foundation/string_id.hpp"
#include "foundation/map.hpp"
#include "mos/meta_attributes.hpp"
#include "mos/meta_property.hpp"
#include "mos/meta_method.hpp"

namespace Engine
{
    namespace Private
    {
        struct MetaObjectInitializer;
    }

    class CORE_API MetaClass : public MetaAttributes
    {
        using Super = MetaAttributes;
        friend struct Private::MetaObjectInitializer;
    public:
        MetaClass(StringID name, EMetaFlag flags, const Map<StringID, String>& attributes)
            : ClassName(name)
            , Super(flags, attributes) {}

        bool FindProperty(StringID name, MetaProperty* property)
        {
            property = Properties.Find(name);
            return property != nullptr;
        }

        bool FindMethod(StringID name, MetaMethod* method)
        {
            method = Methods.Find(name);
            return method != nullptr;
        }

    private:
        MetaClass& AddProperty(MetaProperty&& property) { return *this; }

        MetaClass& AddMethod(MetaMethod&& method) { return *this;}
    private:
        Map<StringID, MetaProperty> Properties;
        Map<StringID, MetaMethod> Methods;
        MetaClass* SuperClass;
        StringID ClassName;
    };

    class CORE_API MetaStruct : public MetaAttributes
    {
    public:
        bool FindProperty(StringID name, MetaProperty* property)
        {
            property = Properties.Find(name);
            return property != nullptr;
        }

    private:
        Map<StringID, MetaProperty> Properties;
    };

    class CORE_API MetaEnum : public MetaAttributes
    {

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

    template <typename T>
    MetaEnum* GetEnum()
    {
        static_assert(FalseExpr<T>, "Can't match MetaEnum");
        return nullptr;
    }
}
