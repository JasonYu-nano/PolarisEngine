#pragma once

#include "foundation/smart_ptr.hpp"
#include "foundation/details/delegate_instance.hpp"

namespace Engine
{
    template <typename RetType, typename... ParamTypes>
    class Delegate
    {
    public:
        using FuncType = RetType(ParamTypes...);

        Delegate(const Delegate& other) = delete;

        Delegate(Delegate&& other) noexcept : Instance(MoveTemp(other.Instance)) {}

        template <typename... VarTypes>
        static Delegate CreateStatic(RetType (*func)(ParamTypes..., VarTypes...), VarTypes&&... vars)
        {
            return Delegate(MakeUnique<StaticDelegateInstance<FuncType, VarTypes...>>(func, Forward<VarTypes>(vars)...));
        }

        template <typename Class, typename... VarTypes>
        static Delegate CreateRaw(Class* obj, typename MemFunPtrType<false, Class, RetType(ParamTypes..., VarTypes...)>::Type fun, VarTypes&&... vars)
        {
            return Delegate(MakeUnique<RawDelegateInstance<false, Class, FuncType, VarTypes...>>(fun, obj, Forward<VarTypes>(vars)...));
        }

        void Unbind()
        {
            Instance.reset();
        }

        RetType Execute(ParamTypes... args)
        {
            return Instance->Execute(Forward<ParamTypes>(args)...);
        }
    private:
        explicit Delegate(UniquePtr<IDelegateInstance<RetType, ParamTypes...>> instance) : Instance(MoveTemp(instance)) {}

        UniquePtr<IDelegateInstance<RetType, ParamTypes...>> Instance;
    };
}