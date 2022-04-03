#include "engine_loop.hpp"
#include "memory/override_new_delete.hpp"


int main(int argv, char** args)
{
    Engine::EngineLoop::Init();

    while (!Engine::EngineLoop::IsExitRequested())
    {
        Engine::EngineLoop::Tick();
    }

    Engine::EngineLoop::Shutdown();
    return 0;
}