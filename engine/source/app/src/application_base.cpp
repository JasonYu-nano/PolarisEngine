#include "application_base.hpp"

namespace Engine
{
    bool ApplicationBase::ExitRequested = false;

    void ApplicationBase::RequestExitApp()
    {
        ExitRequested = true;
    }

    bool ApplicationBase::IsExitRequested()
    {
        return ExitRequested;
    }
}