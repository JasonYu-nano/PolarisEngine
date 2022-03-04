#pragma once

namespace Engine
{
    template <typename RetType>
    struct TypeAutoRegister
    {
        typedef RetType(* Function)();

        explicit TypeAutoRegister(Function function)
        {
            function();
        }
    };
}