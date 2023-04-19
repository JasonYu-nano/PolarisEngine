#pragma once

#include "meta_object.hpp"

namespace Engine
{
    struct OBJECT_API MetaConstructUtils
    {
        static UniquePtr<GMetaClass> ConstructMetaClass(StringID className, GMetaClass* super, EMetaFlag flags);

        static UniquePtr<GMetaStruct> ConstructMetaStruct(StringID structName, GMetaStruct* super, EMetaFlag flags);

        static UniquePtr<GMetaEnum> ConstructMetaEnum(StringID enumName, EMetaFlag flags);

        template <typename RecordType>
        static GMetaProperty* ConstructMetaProperty(RecordType* record, StringID propertyName, size_t offset, EMetaFlag flags)
        {
            GMetaProperty* property = new GMetaProperty(propertyName, offset, flags);
            record->AddProperty(property);
            return property;
        }

        static MetaMethod* ConstructMetaMethod(GMetaClass* metaClass, StringID methodName, MetaMethod::FnPtr fnPtr, EMetaFlag flags);

        static GMetaEnumConstantDecl& ConstructEnumConstantDecl(GMetaEnum* metaEnum, StringID constantName, int64 constantValue, EMetaFlag flags);

#if WITH_META_DATA
        static void SetMetaData(GMetaField* object, const std::initializer_list<Pair<StringID, String>>& metaData);
#endif
    };
}
