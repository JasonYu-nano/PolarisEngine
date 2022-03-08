#include "reflection/reflection_system.hpp"

namespace Engine
{
    ReflectionSystem& ReflectionSystem::Get()
    {
        static ReflectionSystem inst;
        return inst;
    }

    void ReflectionSystem::AddDefferClassRegister(Register func)
    {
        DefferClassRegisters.Add(func);
    }
}
