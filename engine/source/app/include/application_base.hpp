#pragma once
#include "definitions_app.hpp"

namespace Engine
{
    class APP_API ApplicationBase
    {
    public:
        static void RequestExitApp();

        static bool IsExitRequested();

        virtual void Tick() = 0;
    protected:
        static bool ExitRequested;
    };
}