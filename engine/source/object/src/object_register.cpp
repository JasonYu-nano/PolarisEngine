#include "object_register.hpp"
#include "meta_construct_utils.hpp"

DeferInitializer::DeferInitializer(FnInitialize fn)
{
    Array<FnInitialize>& list = Private::GetDeferInitializeList();
    list.Add(fn);
}

UniquePtr<GMetaClass> MetaConstructUtils::ConstructMetaClass(StringID className, GMetaClass* super, EMetaFlag flags)
{
    UniquePtr<GMetaClass> metaClass = MakeUnique<GMetaClass>(className, super, flags);
    return metaClass;
}

UniquePtr<GMetaStruct> MetaConstructUtils::ConstructMetaStruct(StringID structName, GMetaStruct* super, EMetaFlag flags)
{
    UniquePtr<GMetaStruct> metaClass = MakeUnique<GMetaStruct>(structName, super, flags);
    return metaClass;
}

UniquePtr<GMetaEnum> MetaConstructUtils::ConstructMetaEnum(StringID enumName, EMetaFlag flags)
{
    UniquePtr<GMetaEnum> metaClass = MakeUnique<GMetaEnum>(enumName, flags);
    return metaClass;
}

MetaMethod* MetaConstructUtils::ConstructMetaMethod(GMetaClass* metaClass, StringID methodName, MetaMethod::FnPtr fnPtr,
                                             EMetaFlag flags)
{
    MetaMethod method(fnPtr, methodName, flags);
    return metaClass->AddMethod(std::move(method));
}

GMetaEnumConstantDecl& MetaConstructUtils::ConstructEnumConstantDecl(GMetaEnum* metaEnum, StringID constantName, int64 constantValue,
                                              EMetaFlag flags)
{
    return metaEnum->AddConstantDecl(GMetaEnumConstantDecl(constantName, constantValue, flags));
}

#if WITH_META_DATA
void MetaConstructUtils::SetMetaData(GMetaField* object, const std::initializer_list<Pair<StringID, String>>& metaData)
{
    object->SetMetaData(metaData);
}
#endif

Array<FnInitialize>& Private::GetDeferInitializeList()
{
    static Array<FnInitialize> instance;
    return instance;
}
