#pragma once

#include "foundation/tuple.hpp"

namespace Engine
{
    class IDelegateInstance
    {

    };

    template <typename... VarTypes>
    class DelegateInstanceBase : public IDelegateInstance
    {
    public:
        DelegateInstanceBase(VarTypes... vars) : Variables(vars...) {};

    protected:
         Tuple<VarTypes...> Variables;
    };

    template <typename InFuncType, typename... VarTypes>
    class StaticDelegateInstance;

    template <typename RetType, typename... ParamTypes, typename... VarTypes>
    class StaticDelegateInstance<RetType(ParamTypes...), VarTypes...> : public DelegateInstanceBase<VarTypes...>
    {
        using Super = DelegateInstanceBase<VarTypes...>;
    public:
        using FuncType = RetType (*)(ParamTypes...);

        StaticDelegateInstance(FuncType&& func, VarTypes... vars)
            : Super(Forward<VarTypes>(vars)...), Func(Forward<FuncType>(func))
        {}

        RetType Execute(ParamTypes... args)
        {
            this->Variables.ApplyAfter(Func, Forward<ParamTypes>(args)...);
        }

    private:

        FuncType Func;
    };
}