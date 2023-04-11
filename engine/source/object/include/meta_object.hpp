#pragma once

#include "global.hpp"
#include "log/logger.hpp"
#include "foundation/string_id.hpp"
#include "foundation/map.hpp"
#include "meta_property.hpp"
#include "meta_method.hpp"

namespace Engine
{
    class OBJECT_API MetaObject : public MetaField
    {
    public:

    protected:
    };

    class OBJECT_API MetaClass : public MetaObject
    {
        using Super = MetaObject;
        friend struct MetaConstructUtils;
    public:
        MetaClass(StringID name, MetaClass* super, EMetaFlag flags)
            : SuperClass(super)
            , ClassName(name)
        {
            Flags = SuperClass ? static_cast<EMetaFlag>(flags | SuperClass->Flags) : flags;
        }

        bool IsChildOf(MetaClass* metaClass) const;

        bool FindProperty(StringID name, MetaProperty*& property);

        bool FindMethod(StringID name, MetaMethod* method);

    private:
        MetaProperty* AddProperty(MetaProperty&& property);

        MetaMethod* AddMethod(MetaMethod&& method);
    private:
        MetaClass* SuperClass{ nullptr };
        StringID ClassName;
        Array<MetaProperty> Properties;
        Map<StringID, MetaMethod> Methods;
    };

    class OBJECT_API MetaStruct : public MetaObject
    {
        using Super = MetaObject;
        friend struct MetaConstructUtils;
    public:
        MetaStruct(StringID name, MetaStruct* super, EMetaFlag flags)
            : SuperStruct(super)
            , StructName(name)
        {
            Flags = SuperStruct ? static_cast<EMetaFlag>(flags | SuperStruct->Flags) : flags;
        }

        bool IsChildOf(MetaStruct* metaStruct) const;

        bool FindProperty(StringID name, MetaProperty*& property);

    private:
        MetaProperty* AddProperty(MetaProperty&& property);

    private:
        MetaStruct* SuperStruct;
        StringID StructName;
        Array<MetaProperty> Properties;
    };

    class OBJECT_API MetaEnumConstantDecl : public MetaField
    {
        friend class MetaEnum;
    public:
        MetaEnumConstantDecl(StringID constantName, int64 constantValue, EMetaFlag flags)
                : ConstantName(constantName)
                  , ConstantValue(constantValue)
        {
            Flags = flags;
        }

    private:
        StringID ConstantName;
        int64 ConstantValue;
    };

    class OBJECT_API MetaEnum : public MetaField
    {
        friend struct MetaConstructUtils;
    public:
        MetaEnum(StringID enumName, EMetaFlag flags)
                : EnumName(enumName)
        {
            Flags = flags;
        }

        int32 GetEnumSize() const { return ConstantDecl.Size(); }

        int64 GetEnumConstantValueByIndex(int32 index) const;

        StringID GetEnumConstantNameByIndex(int32 index) const;

#if WITH_META_DATA
        String GetEnumConstantDisplayNameByIndex(int32 index) const;
#endif

    private:
        MetaEnumConstantDecl& AddConstantDecl(MetaEnumConstantDecl&& decl);

    private:
        StringID EnumName;
        Array<MetaEnumConstantDecl> ConstantDecl;
    };

    template <typename T>
    MetaEnum* GetEnum()
    {
        static_assert(FalseExpr<T>, "Can't match MetaEnum");
        return nullptr;
    }


    template <typename T>
    inline constexpr bool HasStaticMetaObject = requires(const T& t)
    {
        T::MetaObject();
    };

    template <typename T, bool = HasStaticMetaObject<T> && std::is_base_of_v<class GObject, T>>
    struct HasMetaClass
    {
        static constexpr bool Value = true;
    };

    template <typename T>
    struct HasMetaClass<T, false>
    {
        static constexpr bool Value = false;
    };

    template <typename T>
    inline constexpr bool HasMetaClassV = HasMetaClass<T>::Value;

    template <typename T, bool = HasStaticMetaObject<T> && !HasMetaClassV<T>>
    struct HasMetaStruct
    {
        static constexpr bool Value = true;
    };

    template <typename T>
    struct HasMetaStruct<T, false>
    {
        static constexpr bool Value = false;
    };

    template <typename T>
    inline constexpr bool HasMetaStructV = HasMetaStruct<T>::Value;

    template <typename T, bool = std::is_enum_v<T> && GetEnum<T>>
    struct HasMetaEnum
    {
        static constexpr bool Value = true;
    };

    template <typename T>
    struct HasMetaEnum<T, false>
    {
        static constexpr bool Value = false;
    };

    template <typename T>
    inline constexpr bool HasMetaEnumV = HasMetaEnum<T>::Value;
}
