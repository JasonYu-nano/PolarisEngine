#pragma once

#include "meta_object.hpp"

namespace Engine
{
    struct OBJECT_API MetaConstructUtils
    {
        static UniquePtr<MetaClass> ConstructMetaClass(StringID className, MetaClass* super, EMetaFlag flags);

        static UniquePtr<MetaStruct> ConstructMetaStruct(StringID structName, MetaStruct* super, EMetaFlag flags);

        static UniquePtr<MetaEnum> ConstructMetaEnum(StringID enumName, EMetaFlag flags);

        template <typename RecordType>
        static MetaProperty* ConstructMetaProperty(RecordType* record, StringID propertyName, size_t offset, EMetaFlag flags)
        {
            MetaProperty property(propertyName, offset, flags);
            return record->AddProperty(std::move(property));
        }

        static MetaMethod* ConstructMetaMethod(MetaClass* metaClass, StringID methodName, MetaMethod::FnPtr fnPtr, EMetaFlag flags);

        static MetaEnumConstantDecl& ConstructEnumConstantDecl(MetaEnum* metaEnum, StringID constantName, int64 constantValue, EMetaFlag flags);

#if WITH_META_DATA
        static void SetMetaData(MetaField* object, const std::initializer_list<Pair<StringID, String>>& metaData);
#endif
    };
}
