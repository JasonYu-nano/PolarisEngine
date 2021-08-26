#pragma once
#include "definitions_app.hpp"

namespace Engine
{
    class APP_API ApplicationBase
    {
    public:
        static void RequestExitApp();

        static bool IsExitRequested();
    protected:
        static bool ExitRequested;
    };
}