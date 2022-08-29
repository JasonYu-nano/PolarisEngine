#pragma once

#include "foundation/tuple.hpp"

namespace Engine
{
    template <bool Const, typename Class, typename FuncType>
    struct MemFunPtrType;

    template <typename Class, typename RetType, typename... ArgTypes>
    struct MemFunPtrType<false, Class, RetType(ArgTypes...)>
    {
        typedef RetType (Class::* Type)(ArgTypes...);
    };

    template <typename Class, typename RetType, typename... ArgTypes>
    struct MemFunPtrType<true, Class, RetType(ArgTypes...)>
    {
        typedef RetType (Class::* Type)(ArgTypes...) const;
    };

    template <typename RetType, typename... ArgTypes>
    class IDelegateInstance
    {
    public:
        virtual ~IDelegateInstance() = default;
        virtual RetType Execute(ArgTypes&&... args) = 0;
    };

    template <typename FuncType, typename... VarTypes>
    class DelegateInstanceBase;

    template <typename RetType, typename... ArgTypes, typename... VarTypes>
    class DelegateInstanceBase<RetType(ArgTypes...), VarTypes...> : public IDelegateInstance<RetType, ArgTypes...>
    {
    public:
        DelegateInstanceBase(VarTypes&&... vars) : Variables(Forward<VarTypes>(vars)...) {};

    protected:
        Tuple<VarTypes...> Variables;
    };

    template <typename InFuncType, typename... VarTypes>
    class StaticDelegateInstance;

    template <typename RetType, typename... ArgTypes, typename... VarTypes>
    class StaticDelegateInstance<RetType(ArgTypes...), VarTypes...> : public DelegateInstanceBase<RetType(ArgTypes...), VarTypes...>
    {
        using Super = DelegateInstanceBase<RetType(ArgTypes...), VarTypes...>;
    public:
        using FunType = RetType(ArgTypes..., VarTypes...);

        StaticDelegateInstance(FunType* func, VarTypes... vars)
            : Super(Forward<VarTypes>(vars)...), Func(func)
        {}

        virtual RetType Execute(ArgTypes&&... args)
        {
            return this->Variables.ApplyAfter(Func, Forward<ArgTypes>(args)...);
        }

    private:

        FunType* Func;
    };

    template <bool Const, typename Class, typename InFuncType, typename... VarTypes>
    class RawDelegateInstance;

    template <bool Const, typename Class, typename RetType, typename... ArgTypes, typename... VarTypes>
    class RawDelegateInstance<Const, Class, RetType(ArgTypes...), VarTypes...> : public DelegateInstanceBase<RetType(ArgTypes...), VarTypes...>
    {
        using Super = DelegateInstanceBase<RetType(ArgTypes...), VarTypes...>;
    public:
        using FunPtr = typename MemFunPtrType<Const, Class, RetType(ArgTypes..., VarTypes...)>::Type;

        RawDelegateInstance(FunPtr fun, Class* obj, VarTypes... vars)
            : Super(Forward<VarTypes>(vars)...), Obj(obj), Fun(fun)
        {}

        virtual RetType Execute(ArgTypes&&... args)
        {
            return this->Variables.ApplyAfter(Fun, Obj, Forward<ArgTypes>(args)...);
        }

    private:
        Class* Obj;

        FunPtr Fun;
    };
}