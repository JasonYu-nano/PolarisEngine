#include "mos/meta_register.hpp"

void MetaObjectRegistry::Initialize()
{
    Private::MetaObjectInitializer::Initialize();
}

void MetaObjectRegistry::Finalize()
{

}

MetaClass* MetaObjectRegistry::GetMetaClassChecked(StringID className)
{
    return nullptr;
}

Array<StructureRegister*>  Private::MetaObjectInitializer::DeferRegisterMetaClassArray;

void Private::MetaObjectInitializer::Initialize()
{
    for (auto&& inst : DeferRegisterMetaClassArray)
    {
        inst->Register();
    }
}

void Private::MetaObjectInitializer::CollectDeferRegisterMetaClass(StructureRegister* inst)
{
    DeferRegisterMetaClassArray.Add(inst);
}
