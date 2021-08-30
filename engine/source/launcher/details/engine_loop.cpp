#include "engine_loop.hpp"
#include "platform_application.hpp"

namespace Engine
{
    void EngineLoop::Init()
    {
        auto* app = PlatformApplication::CreateApplication();
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
        PlatformApplication::DestroyApplication();
    }

    bool EngineLoop::IsExitRequested()
    {
        return PlatformApplication::IsExitRequested();
    }
}