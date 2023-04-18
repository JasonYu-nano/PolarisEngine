#pragma once

#include "global.hpp"
#include "log/logger.hpp"
#include "foundation/string_id.hpp"
#include "foundation/map.hpp"
#include "meta_property.hpp"
#include "meta_method.hpp"
#include "meta_object.gen.hpp"

namespace Engine
{
    class OBJECT_API META() GMetaObject : public GMetaField
    {
        MOC_GENERATED()
    };

    class OBJECT_API META() GMetaClass : public GMetaObject
    {
        MOC_GENERATED()
        friend struct MetaConstructUtils;
    public:
        GMetaClass(StringID name, GMetaClass* super, EMetaFlag flags)
            : SuperClass(super)
            , ClassName(name)
        {
            Flags = SuperClass ? static_cast<EMetaFlag>(flags | SuperClass->Flags) : flags;
        }

        ~GMetaClass() override;

        bool IsChildOf(GMetaClass* metaClass) const;

        bool FindProperty(StringID name, MetaProperty*& property);

        bool FindMethod(StringID name, MetaMethod* method);

    private:
        void AddProperty(MetaProperty* property);

        MetaMethod* AddMethod(MetaMethod&& method);
    private:
        GMetaClass* SuperClass{ nullptr };
        StringID ClassName;
        Array<MetaProperty*> Properties;
        Map<StringID, MetaMethod> Methods;
    };

    class OBJECT_API META() GMetaStruct : public GMetaObject
    {
        MOC_GENERATED()
        friend struct MetaConstructUtils;
    public:
        GMetaStruct(StringID name, GMetaStruct* super, EMetaFlag flags)
            : SuperStruct(super)
            , StructName(name)
        {
            Flags = SuperStruct ? static_cast<EMetaFlag>(flags | SuperStruct->Flags) : flags;
        }

        ~GMetaStruct() override;

        bool IsChildOf(GMetaStruct* metaStruct) const;

        bool FindProperty(StringID name, MetaProperty*& property);

    private:
        void AddProperty(MetaProperty* property);

    private:
        GMetaStruct* SuperStruct;
        StringID StructName;
        Array<MetaProperty*> Properties;
    };

    class OBJECT_API META() GMetaEnumConstantDecl : public GMetaField
    {
        MOC_GENERATED()
        friend class GMetaEnum;
    public:
        GMetaEnumConstantDecl(StringID constantName, int64 constantValue, EMetaFlag flags)
            : ConstantName(constantName)
            , ConstantValue(constantValue)
        {
            Flags = flags;
        }

    private:
        StringID ConstantName;
        int64 ConstantValue;
    };

    class OBJECT_API META() GMetaEnum : public GMetaField
    {
        MOC_GENERATED()
        friend struct MetaConstructUtils;
    public:
        GMetaEnum(StringID enumName, EMetaFlag flags)
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
        GMetaEnumConstantDecl& AddConstantDecl(GMetaEnumConstantDecl&& decl);

    private:
        StringID EnumName;
        Array<GMetaEnumConstantDecl> ConstantDecl;
    };

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
