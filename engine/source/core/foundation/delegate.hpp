#pragma once

#include "foundation/smart_ptr.hpp"
#include "foundation/details/delegate_instance.hpp"

namespace Engine
{
    template <typename RetType, typename... ArgTypes>
    class Delegate
    {
    public:
        using FuncType = RetType(ArgTypes...);

        Delegate(const Delegate& other) = delete;

        Delegate(Delegate&& other) noexcept : Instance(MoveTemp(other.Instance)) {}

        Delegate& operator= (Delegate&& other)
        {
            Instance = MoveTemp(other.Instance);
            return *this;
        }

        template <typename... VarTypes>
        static Delegate CreateStatic(RetType (*fun)(ArgTypes..., VarTypes...), VarTypes&&... vars)
        {
            return Delegate(MakeUnique<StaticDelegateInstance<FuncType, VarTypes...>>(fun, Forward<VarTypes>(vars)...));
        }

        template <typename Class, typename... VarTypes>
        static Delegate CreateRaw(Class* obj, typename MemFunPtrType<false, Class, RetType(ArgTypes..., VarTypes...)>::Type fun, VarTypes&&... vars)
        {
            return Delegate(MakeUnique<RawDelegateInstance<false, Class, FuncType, VarTypes...>>(fun, obj, Forward<VarTypes>(vars)...));
        }

        template <typename Class, typename... VarTypes>
        static Delegate CreateRaw(Class* obj, typename MemFunPtrType<true, Class, RetType(ArgTypes..., VarTypes...)>::Type fun, VarTypes&&... vars)
        {
            return Delegate(MakeUnique<RawDelegateInstance<true, Class, FuncType, VarTypes...>>(fun, obj, Forward<VarTypes>(vars)...));
        }

        template <typename Class, typename... VarTypes>
        static Delegate CreateSP(SharedPtr<Class> obj, typename MemFunPtrType<false, Class, RetType(ArgTypes..., VarTypes...)>::Type fun, VarTypes&&... vars)
        {
            return Delegate(MakeUnique<SPDelegateInstance<false, Class, FuncType, VarTypes...>>(fun, obj, Forward<VarTypes>(vars)...));
        }

        template <typename Class, typename... VarTypes>
        static Delegate CreateSP(SharedPtr<Class> obj, typename MemFunPtrType<true, Class, RetType(ArgTypes..., VarTypes...)>::Type fun, VarTypes&&... vars)
        {
            return Delegate(MakeUnique<SPDelegateInstance<true, Class, FuncType, VarTypes...>>(fun, obj, Forward<VarTypes>(vars)...));
        }

        void Unbind()
        {
            Instance.reset();
        }

        RetType Execute(ArgTypes... args)
        {
            return Instance->Execute(Forward<ArgTypes>(args)...);
        }
    private:
        explicit Delegate(UniquePtr<IDelegateInstance<RetType, ArgTypes...>> instance) : Instance(MoveTemp(instance)) {}

        UniquePtr<IDelegateInstance<RetType, ArgTypes...>> Instance;
    };
}