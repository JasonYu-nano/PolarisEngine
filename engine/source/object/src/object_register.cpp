#include "object_register.hpp"
#include "meta_construct_utils.hpp"

DeferInitializer::DeferInitializer(FnInitialize fn)
{
    Array<FnInitialize>& list = Private::GetDeferInitializeList();
    list.Add(fn);
}

UniquePtr<MetaClass> MetaConstructUtils::ConstructMetaClass(StringID className, MetaClass* super, EMetaFlag flags)
{
    UniquePtr<MetaClass> metaClass = MakeUnique<MetaClass>(className, super, flags);
    return metaClass;
}

UniquePtr<MetaStruct> MetaConstructUtils::ConstructMetaStruct(StringID structName, MetaStruct* super, EMetaFlag flags)
{
    UniquePtr<MetaStruct> metaClass = MakeUnique<MetaStruct>(structName, super, flags);
    return metaClass;
}

UniquePtr<MetaEnum> MetaConstructUtils::ConstructMetaEnum(StringID enumName, EMetaFlag flags)
{
    UniquePtr<MetaEnum> metaClass = MakeUnique<MetaEnum>(enumName, flags);
    return metaClass;
}

MetaMethod* MetaConstructUtils::ConstructMetaMethod(MetaClass* metaClass, StringID methodName, MetaMethod::FnPtr fnPtr,
                                             EMetaFlag flags)
{
    MetaMethod method(fnPtr, methodName, flags);
    return metaClass->AddMethod(std::move(method));
}

MetaEnumConstantDecl& MetaConstructUtils::ConstructEnumConstantDecl(MetaEnum* metaEnum, StringID constantName, int64 constantValue,
                                              EMetaFlag flags)
{
    return metaEnum->AddConstantDecl(MetaEnumConstantDecl(constantName, constantValue, flags));
}

#if WITH_META_DATA
void MetaConstructUtils::SetMetaData(MetaField* object, const std::initializer_list<Pair<StringID, String>>& metaData)
{
    object->SetMetaData(metaData);
}
#endif

Array<FnInitialize>& Private::GetDeferInitializeList()
{
    static Array<FnInitialize> instance;
    return instance;
}
