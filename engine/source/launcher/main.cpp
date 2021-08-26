#include "engine_loop.hpp"


int main(void* args, int argv)
{
    Engine::EngineLoop::Init();

    if (!Engine::EngineLoop::IsExitRequested())
    {
        Engine::EngineLoop::Tick();
    }

    Engine::EngineLoop::Shutdown();
    return 0;
}