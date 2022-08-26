#pragma once

namespace Engine
{
    template <typename RetType, typename... ParamTypes>
    class Delegate
    {
    public:
        template <typename... VarTypes>
        static Delegate CreateStatic(RetType (*)(ParamTypes..., VarTypes...));
    };
}