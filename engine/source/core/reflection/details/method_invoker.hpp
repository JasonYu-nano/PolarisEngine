#pragma once

#include "reflection/variant.hpp"

namespace Engine
{
    constexpr int32 kMaxParamNum = 10;

    class ParamPack
    {
    public:
        ParamPack(std::initializer_list<Variant> initializer)
        {
            if (initializer.size() > kMaxParamNum)
            {
                PL_ERROR("Engine", _T("Arguments count over max size"));
                return;
            }

            int32 idx = 0;
            for (auto&& var : initializer)
            {
                Params[idx] = MoveTemp(var);
                ++idx;
            }

            Count = idx;
        }

        Variant& operator[] (int32 idx)
        {
            PL_ASSERT(idx >= 0 && idx < kMaxParamNum);
            return Params[idx];
        }

        const Variant& operator[] (int32 idx) const
        {
            PL_ASSERT(idx >= 0 && idx < kMaxParamNum);
            return Params[idx];
        }

        int32 GetCount() const
        {
            return Count;
        }

    private:
        int32 Count{ 0 };
        Variant Params[kMaxParamNum];
    };

    #define INVOKE_RETURN_VARIANT(RetType, ret) \
        if constexpr(std::is_reference_v<RetType>) \
        { \
            return Variant(ret); \
        } \
        else \
        { \
            return Variant(ret, VARIANT_COPY); \
        }

    class IMethodInvoker
    {
    public:
        virtual Variant Invoke(Variant obj) = 0;

        virtual Variant Invoke(Variant obj, const ParamPack& params) = 0;

        virtual int32 GetParamCount() const = 0;
    };

    template <typename ClassType, typename RetType, typename... Args>
    class MethodInvoker : public IMethodInvoker
    {
        typedef RetType(ClassType::* Func)(Args...);
        typedef RetType(ClassType::* ConstFunc)(Args...) const;

    public:
        MethodInvoker(Func func) : Function(reinterpret_cast<ConstFunc>(func)) {}

        MethodInvoker(ConstFunc func) : Function(func) {}

        Variant Invoke(Variant obj) final
        {
            if constexpr(sizeof...(Args) == 0)
            {
                auto&& ret = (obj.GetValue<ClassType>().*Function)();
                INVOKE_RETURN_VARIANT(RetType, ret);
            }

            return {};
        }

        Variant Invoke(Variant obj, const ParamPack& params) final
        {
            if (params.GetCount() != sizeof...(Args))
            {
                PL_ERROR("Engine", _T("Argument count not match the method"));
                return {};
            }

            return InvokeImpl<void, Args...>(obj, params);
        }

        int32 GetParamCount() const final
        {
            return sizeof...(Args);
        }

    private:
        template <typename _>
        Variant InvokeImpl(const Variant& obj, const ParamPack& params)
        {
            auto&& ret = (obj.GetValue<ClassType>().*Function)();
            INVOKE_RETURN_VARIANT(RetType, ret);
        }

        template <typename _, typename Arg0>
        Variant InvokeImpl(const Variant& obj, const ParamPack& params)
        {
            auto&& ret = (obj.GetValue<ClassType>().*Function)(params[0].GetValue<Arg0>());
            INVOKE_RETURN_VARIANT(RetType, ret);
        }

    private:
        template <typename Arg>
        Variant InvokeImpl(Arg param)
        {
            return Variant(Function(param));
        }

    private:
        ConstFunc Function;
    };

    template <typename ClassType, typename... Args>
    class MethodInvoker<ClassType, void, Args...> : public IMethodInvoker
    {
        typedef void(ClassType::* Func)(Args...);
        typedef void(ClassType::* ConstFunc)(Args...) const;

    public:
        MethodInvoker(Func func) : Function(reinterpret_cast<ConstFunc>(func)) {}

        MethodInvoker(ConstFunc func) : Function(func) {}

        Variant Invoke(Variant obj) final
        {
            if constexpr(sizeof...(Args) == 0)
            {
                (obj.GetValue<ClassType>().*Function)();
            }

            return {};
        }

        Variant Invoke(Variant obj, const ParamPack& params) final
        {
            if (params.GetCount() != sizeof...(Args))
            {
                PL_ERROR("Engine", _T("Argument count not match the method"));
                return {};
            }

            InvokeImpl<void, Args...>(obj, params);
            return {};
        }

        int32 GetParamCount() const final
        {
            return sizeof...(Args);
        }

    private:
        template <typename _>
        void InvokeImpl(const Variant& obj, const ParamPack& params)
        {
            (obj.GetValue<ClassType>().*Function)();
        }

        template <typename _, typename Arg0>
        void InvokeImpl(const Variant& obj, const ParamPack& params)
        {
            (obj.GetValue<ClassType>().*Function)(params[0].GetValue<Arg0>());
        }

    private:
        ConstFunc Function;
    };
}