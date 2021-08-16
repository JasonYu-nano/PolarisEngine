#include "app/application.hpp"

namespace Engine
{
    void Application::Init()
    {
        Driver = AppDriverHelper::GetDriver();
    }

    void Application::Shutdown()
    {

    }
}