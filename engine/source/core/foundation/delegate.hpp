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

        Delegate& operator= (Delegate&& other) noexcept
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

        template <typename... VarTypes>
        void BindStatic(RetType (*fun)(ArgTypes..., VarTypes...), VarTypes&&... vars)
        {
            *this = CreateStatic(fun, Forward<VarTypes>(vars)...);
        }

        template <typename Class, typename... VarTypes>
        void BindRaw(Class* obj, typename MemFunPtrType<false, Class, RetType(ArgTypes..., VarTypes...)>::Type fun, VarTypes&&... vars)
        {
            *this = CreateRaw(obj, fun, Forward<VarTypes>(vars)...);
        }

        template <typename Class, typename... VarTypes>
        void BindRaw(Class* obj, typename MemFunPtrType<true, Class, RetType(ArgTypes..., VarTypes...)>::Type fun, VarTypes&&... vars)
        {
            *this = CreateRaw(obj, fun, Forward<VarTypes>(vars)...);
        }

        template <typename Class, typename... VarTypes>
        void BindSP(SharedPtr<Class> obj, typename MemFunPtrType<false, Class, RetType(ArgTypes..., VarTypes...)>::Type fun, VarTypes&&... vars)
        {
            *this = CreateSP(obj, fun, Forward<VarTypes>(vars)...);
        }

        template <typename Class, typename... VarTypes>
        void BindSP(SharedPtr<Class> obj, typename MemFunPtrType<true, Class, RetType(ArgTypes..., VarTypes...)>::Type fun, VarTypes&&... vars)
        {
            *this = CreateSP(obj, fun, Forward<VarTypes>(vars)...);
        }

        void Unbind()
        {
            Instance.reset();
        }

        bool IsBound() const
        {
            return Instance != nullptr && Instance->IsSafeToExecute();
        }

        RetType Execute(ArgTypes... args)
        {
            return Instance->Execute(Forward<ArgTypes>(args)...);
        }

        bool ExecuteIfBound(ArgTypes... args)
        {
            if (!IsBound())
            {
                return false;
            }

            Instance->Execute(Forward<ArgTypes>(args)...);
            return true;
        }

    private:
        explicit Delegate(UniquePtr<IDelegateInstance<RetType, ArgTypes...>> instance) : Instance(MoveTemp(instance)) {}

        UniquePtr<IDelegateInstance<RetType, ArgTypes...>> Instance;
    };
}