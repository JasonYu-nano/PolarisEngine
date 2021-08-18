#include "app/application.hpp"
#include "render_module.hpp"

namespace Engine
{
    void Application::Init()
    {
        Driver = AppDriverHelper::GetDriver();
        Driver->Init();
        RenderModule::Get().Init();
    }

    void Application::Shutdown()
    {
        RenderModule::Get().Shutdown();
        Driver->Shutdown();
    }

    void Application::Tick()
    {
        Driver->Tick();
    }
}