#include "app/application.hpp"

namespace Engine
{
    void Application::Init()
    {
        Driver = AppDriverHelper::GetDriver();
        Driver->Init();
    }

    void Application::Shutdown()
    {
        Driver->Shutdown();
    }
    void Application::Tick()
    {
        Driver->Tick();
    }
}