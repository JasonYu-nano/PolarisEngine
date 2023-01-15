#pragma once

#include "definitions_core.hpp"
#include "global.hpp"
#include "module/module_interface.hpp"
#include "foundation/fixed_string.hpp"

namespace Engine
{
    template <typename T>
    concept ModuleType = std::is_base_of_v<IModuleInterface, T>;

    class CORE_API ModuleManager
    {
    public:
        static ModuleManager& Get();

        template <ModuleType Module>
        static Module* Load(const FixedString& name)
        {
            if (IModuleInterface* module = Get().LoadImpl<Module>(name))
            {
                return static_cast<Module*>(module);
            }
            return nullptr;
        }

        static void ShutdownModule()
        {
            Get().Shutdown();
        }
    private:
        template <ModuleType Module>
        IModuleInterface* LoadImpl(const FixedString& name)
        {
            IModuleInterface* module;
            {
                std::shared_lock lock(ModuleRWMutex);
                module = FindModule(name);
            }
            if (module)
            {
                return module;
            }
            else
            {
                std::lock_guard lock(ModuleRWMutex);
                module = FindModule(name);

                if (!module)
                {
                    module = new Module();
                    module->Startup();
                    CachedModule.Add(name, module);
                }
                return module;
            }
        }

        IModuleInterface* FindModule(const FixedString& name);

        void Shutdown();

        Map<FixedString, IModuleInterface*> CachedModule;
        std::shared_mutex ModuleRWMutex;
    };
}