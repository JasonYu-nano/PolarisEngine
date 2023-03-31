#include "object_module.hpp"
#include "object_register.hpp"

void ObjectModule::Startup()
{
    Array<FnInitialize>& initializerList = Private::GetDeferInitializeList();
    for (auto&& initializer : initializerList)
    {
        (*initializer)();
    }
}

void ObjectModule::Shutdown()
{

}
