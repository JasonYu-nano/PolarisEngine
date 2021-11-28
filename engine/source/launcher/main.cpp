#include "engine_loop.hpp"
#include "memory/override_new_delete.hpp"


int main(void* args, int argv)
{
    Engine::EngineLoop::Init();

    while (!Engine::EngineLoop::IsExitRequested())
    {
        Engine::EngineLoop::Tick();
    }

    Engine::EngineLoop::Shutdown();
    return 0;
}