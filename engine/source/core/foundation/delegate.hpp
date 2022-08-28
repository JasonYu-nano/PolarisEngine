#pragma once

#include "foundation/details/delegate_instance.hpp"

namespace Engine
{
    template <typename RetType, typename... ParamTypes>
    class Delegate
    {
    public:
        template <typename... VarTypes>
        static Delegate CreateStatic(RetType (*)(ParamTypes..., VarTypes...));

    private:
        IDelegateInstance* Instance;
    };
}