#include "module/module_manager.hpp"

namespace Engine
{
    ModuleManager& ModuleManager::Get()
    {
        static ModuleManager inst;
        return inst;
    }

    IModuleInterface* ModuleManager::FindModule(const FixedString &name)
    {
        std::shared_lock lock(ModuleRWMutex);
        if (IModuleInterface** module = CachedModule.Find(name))
        {
            return *module;
        }
        return nullptr;
    }

    void ModuleManager::Shutdown()
    {
        for (auto&& pair : CachedModule)
        {
            pair.Value->Shutdown();
        }

        for (auto&& pair : CachedModule)
        {
            delete pair.Value;
        }
        CachedModule.Clear();
    }
}