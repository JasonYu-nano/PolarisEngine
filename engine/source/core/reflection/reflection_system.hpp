#pragma once

#include "core_minimal_private.hpp"
#include "reflection/structure.hpp"

namespace Engine
{
    class ReflectionSystem
    {
    public:
        typedef Structure*(* Register)();
    public:
        static ReflectionSystem& Get();

        void AddDefferClassRegister(Register func);
    private:
        DynamicArray<Register> DefferClassRegisters;
    };
}