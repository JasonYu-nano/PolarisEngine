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
            static_assert(!std::is_const_v<Class>, "Can't bind none const member function with const object");
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
            static_assert(!std::is_const_v<Class>, "Can't bind none const member function with const object");
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

        DelegateHandle GetHandle() const
        {
            return Instance->GetHandle();
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

    template <typename RetType, typename... ArgTypes>
    class MultiDelegate
    {
        static_assert(sizeof(RetType) == 0, "Return type must be void");
    };

    template <typename... ArgTypes>
    class MultiDelegate<void, ArgTypes...>
    {
        using DelegateType = Delegate<void, ArgTypes...>;
        using InstanceType = IDelegateInstance<void, ArgTypes...>;
    public:
        ~MultiDelegate() = default;

        DelegateHandle Add(DelegateType&& delegate)
        {
            DelegateHandle handle = delegate.GetHandle();
            DelegateArray.Add(Forward<DelegateType>(delegate));
            return handle;
        }

        void Clear()
        {
            DelegateArray.Clear();
        }

        template <typename... VarTypes>
        DelegateHandle AddStatic(void (*fun)(ArgTypes..., VarTypes...), VarTypes&&... vars)
        {
            return Add(DelegateType::template CreateStatic<VarTypes...>(fun, Forward<VarTypes>(vars)...));
        }

        template <typename Class, typename... VarTypes>
        DelegateHandle AddRaw(Class* obj, typename MemFunPtrType<false, Class, void(ArgTypes..., VarTypes...)>::Type fun, VarTypes&&... vars)
        {
            return Add(DelegateType::template CreateRaw<Class, VarTypes...>(obj, fun, Forward<VarTypes>(vars)...));
        }

        template <typename Class, typename... VarTypes>
        DelegateHandle AddRaw(Class* obj, typename MemFunPtrType<true, Class, void(ArgTypes..., VarTypes...)>::Type fun, VarTypes&&... vars)
        {
            return Add(DelegateType::template CreateRaw<Class, VarTypes...>(obj, fun, Forward<VarTypes>(vars)...));
        }

        template <typename Class, typename... VarTypes>
        DelegateHandle AddSP(SharedPtr<Class> obj, typename MemFunPtrType<false, Class, void(ArgTypes..., VarTypes...)>::Type fun, VarTypes&&... vars)
        {
            return Add(DelegateType::template CreateSP<Class, VarTypes...>(obj, fun, Forward<VarTypes>(vars)...));
        }

        template <typename Class, typename... VarTypes>
        DelegateHandle AddSP(SharedPtr<Class> obj, typename MemFunPtrType<true, Class, void(ArgTypes..., VarTypes...)>::Type fun, VarTypes&&... vars)
        {
            return Add(DelegateType::template CreateSP<Class, VarTypes...>(obj, fun, Forward<VarTypes>(vars)...));
        }

        void Remove(const DelegateHandle& handle)
        {
            for (auto it = DelegateArray.rbegin(); (bool)it; --it)
            {
                if (it->GetHandle() == handle)
                {
                    if (LockCounter > 0)
                    {
                        it->Unbind();
                    }
                    else
                    {
                        DelegateArray.RemoveAtSwap(it.GetIndex());
                    }
                    break;
                }
            }
        }

        inline bool IsBound() const
        {
            return DelegateArray.Size() > 0;
        }

        void Broadcast(ArgTypes... args)
        {
            AddLock();
            for (auto it = DelegateArray.rbegin(); (bool)it; --it)
            {
                it->ExecuteIfBound(Forward<ArgTypes>(args)...);
            }
            RemoveLock();
        }

        void BroadcastIfBound(ArgTypes... args)
        {
            if (IsBound())
            {
                Broadcast(Forward<ArgTypes>(args)...);
            }
        }
    protected:
        void AddLock() { ++LockCounter; }

        void RemoveLock() { --LockCounter; }
    protected:
        int8 LockCounter{ 0 };

        DynamicArray<DelegateType> DelegateArray;
    };


    #define DECLARE_DELEGATE(alias) using alias = Delegate<void>
    #define DECLARE_DELEGATE_ONE_PARAM(alias, type1) using alias = Delegate<void, type1>
    #define DECLARE_DELEGATE_TWO_PARAMS(alias, type1, type2) using alias = Delegate<void, type1, type2>

    #define DECLARE_DELEGATE_RET(alias, ret) using alias = Delegate<ret>
    #define DECLARE_DELEGATE_ONE_PARAM_RET(alias, ret, type1) using alias = Delegate<ret, type1>
    #define DECLARE_DELEGATE_TWO_PARAMS_RET(alias, ret, type1, type2) using alias = Delegate<ret, type1, type2>

    #define DECLARE_MULTI_DELEGATE(alias) using alias = MultiDelegate<void>
    #define DECLARE_MULTI_DELEGATE_ONE_PARAM(alias, type1) using alias = MultiDelegate<void, type1>
    #define DECLARE_MULTI_DELEGATE_TWO_PARAMS(alias, type1, type2) using alias = MultiDelegate<void, type1, type2>
}