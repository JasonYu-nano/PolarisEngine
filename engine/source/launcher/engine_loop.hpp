#pragma once

namespace Engine
{
    class EngineLoop
    {
    public:
        static void Init();

        static void Tick();

        static void Shutdown();

        static bool IsExitRequested();
    };
}