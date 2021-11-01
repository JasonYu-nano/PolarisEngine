#include "engine_loop.hpp"
#include "platform_application.hpp"
#include "core/memory/memory.hpp"

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
        Memory::Shutdown();
    }

    bool EngineLoop::IsExitRequested()
    {
        return PlatformApplication::IsExitRequested();
    }
}