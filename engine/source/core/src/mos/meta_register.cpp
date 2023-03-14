#include "mos/meta_register.hpp"

void MetaObjectRegistry::Initialize()
{
    Private::MetaObjectInitializer::Initialize();
}

void MetaObjectRegistry::Finalize()
{

}

Map<StringID, MetaClass*> MetaObjectRegistry::ClassMap;

MetaClass* MetaObjectRegistry::GetMetaClassChecked(StringID className)
{
    return *ClassMap.Find(className);
}

Array<StructureRegister*>  Private::MetaObjectInitializer::DeferRegisterMetaClassArray;

void Private::MetaObjectInitializer::Initialize()
{
    for (auto&& inst : DeferRegisterMetaClassArray)
    {
        MetaObjectRegistry::ClassMap.Add(inst->NameID, reinterpret_cast<MetaClass*>(Memory::Malloc(sizeof(MetaClass))));
        inst->Register();
    }
}

void Private::MetaObjectInitializer::CollectDeferRegisterMetaClass(StructureRegister* inst)
{
    DeferRegisterMetaClassArray.Add(inst);
}

MetaClass* Private::MetaObjectInitializer::ConstructMetaClass(StringID className, EMetaFlag flag, Map<StringID, String>&& attributes)
{
    MetaClass* metaClass = MetaObjectRegistry::GetMetaClassChecked(className);
    new(metaClass) MetaClass(className, flag, std::forward<Map<StringID, String>&&>(attributes));
    return metaClass;
}
