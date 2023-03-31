#pragma once

#include "definitions_object.hpp"
#include "meta_structure.hpp"
#include "meta_enum.hpp"

namespace Engine
{
    typedef void(* FnInitialize)();

    struct OBJECT_API DeferInitializer
    {
        explicit DeferInitializer(FnInitialize fn);
    };

    template <typename Type>
    void RegisterMetaObject()
    {
        Type::MetaObject();
    }

    template <typename Type>
    void RegisterMetaEnum()
    {
        GetEnum<Type>();
    }

    struct OBJECT_API MetaConstructUtils
    {
        static UniquePtr<MetaClass> ConstructMetaClass(StringID className, MetaClass* super, EMetaFlag flags);

        static UniquePtr<MetaStruct> ConstructMetaStruct(StringID structName, MetaStruct* super, EMetaFlag flags);

        static UniquePtr<MetaEnum> ConstructMetaEnum(StringID enumName, EMetaFlag flags);

        template <typename RecordType>
        static MetaProperty& ConstructMetaProperty(RecordType* record, StringID propertyName, size_t offset, EMetaFlag flags)
        {
            MetaProperty property(propertyName, offset, flags);
            return record->AddProperty(std::move(property));
        }

        static MetaMethod& ConstructMetaMethod(MetaClass* metaClass, StringID methodName, MetaMethod::FnPtr fnPtr, EMetaFlag flags);

        static MetaEnumConstantDecl& ConstructEnumConstantDecl(MetaEnum* metaEnum, StringID constantName, int64 constantValue, EMetaFlag flags);

#if WITH_META_DATA
        static void SetMetaData(MetaObject* object, const std::initializer_list<Pair<StringID, String>>& metaData);
#endif
    };

namespace Private
{
    inline Array<FnInitialize>& GetDeferInitializeList();
}
}
