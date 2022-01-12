#include "engine_loop.hpp"
#include "platform_application.hpp"
#include "memory/memory.hpp"
#include "render_module.hpp"

namespace Engine
{
    void EngineLoop::Init()
    {
        auto* app = PlatformApplication::CreateApplication();
        RenderModule::Get().Init();
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
        RenderModule::Get().Shutdown();
        PlatformApplication::DestroyApplication();
        Memory::Shutdown();
    }

    bool EngineLoop::IsExitRequested()
    {
        return PlatformApplication::IsExitRequested();
    }
}