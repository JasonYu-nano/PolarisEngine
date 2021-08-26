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