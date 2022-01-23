#include "engine_loop.hpp"
#include "platform_application.hpp"
#include "memory/memory.hpp"
#include "render_module.hpp"
#include "module/module_manager.hpp"

namespace Engine
{
    void EngineLoop::Init()
    {
        auto* app = PlatformApplication::CreateApplication();
        ModuleManager::Load<RenderModule>(_T("Render"));
    }

    void EngineLoop::Tick()
    {
        auto* app = PlatformApplication::GetApplication();
        if (app != nullptr)
        {
            app->Tick();
        }
    }

    void EngineLoop::Shutdown()
    {
        ModuleManager::ShutdownModule();
        PlatformApplication::DestroyApplication();
        Memory::Shutdown();
    }

    bool EngineLoop::IsExitRequested()
    {
        return PlatformApplication::IsExitRequested();
    }
}