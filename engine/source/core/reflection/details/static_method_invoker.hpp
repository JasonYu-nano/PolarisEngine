#pragma once

#include "reflection/details/method_invoker_interface.hpp"

namespace Engine
{
    template <typename RetType, typename... Args>
    class StaticMethodInvoker : public IMethodInvoker
    {
        typedef RetType(* Func)(Args...);

    public:
        explicit StaticMethodInvoker(Func func) : Function(func) {}

        Variant Invoke(const ParamPack &params) final
        {
            if (params.GetCount() != sizeof...(Args))
            {
                PL_ERROR("Engine", _T("Argument count not match the method"));
                return {};
            }

            return InvokeImpl<void, Args...>(params);
        }

        int32 GetParamCount() const final
        {
            return sizeof...(Args);
        }

        bool IsStatic() const final
        {
            return true;
        }

    private:
        template <typename _>
        Variant InvokeImpl(const ParamPack& params)
        {
            auto&& ret = (*Function)();
            INVOKE_RETURN_VARIANT(RetType, ret);
        }

        template <typename _, typename Arg0>
        Variant InvokeImpl(const ParamPack& params)
        {
            auto&& ret = (*Function)(params[0].GetValue<Arg0>());
            INVOKE_RETURN_VARIANT(RetType, ret);
        }

        template <typename _, typename Arg0, typename Arg1>
        Variant InvokeImpl(const ParamPack& params)
        {
            auto&& ret = (*Function)(params[0].GetValue<Arg0>(), params[1].GetValue<Arg1>());
            INVOKE_RETURN_VARIANT(RetType, ret);
        }

        template <typename _, typename Arg0, typename Arg1, typename Arg2>
        Variant InvokeImpl(const ParamPack& params)
        {
            auto&& ret = (*Function)(params[0].GetValue<Arg0>(), params[1].GetValue<Arg1>(), params[2].GetValue<Arg2>());
            INVOKE_RETURN_VARIANT(RetType, ret);
        }

    private:
        Func Function;
    };

    template <typename... Args>
    class StaticMethodInvoker<void, Args...> : public IMethodInvoker
    {
        typedef void(* Func)(Args...);

    public:
        explicit StaticMethodInvoker(Func func) : Function(func) {}

        Variant Invoke(const ParamPack &params) final
        {
            if (params.GetCount() != sizeof...(Args))
            {
                PL_ERROR("Engine", _T("Argument count not match the method"));
            }
            else
            {
                InvokeImpl<void, Args...>(params);
            }

            return {};
        }

        int32 GetParamCount() const final
        {
            return sizeof...(Args);
        }

        bool IsStatic() const final
        {
            return true;
        }

    private:
        template <typename _>
        void InvokeImpl(const ParamPack& params)
        {
            (*Function)();
        }

        template <typename _, typename Arg0>
        void InvokeImpl(const ParamPack& params)
        {
            (*Function)(params[0].GetValue<Arg0>());
        }

        template <typename _, typename Arg0, typename Arg1>
        void InvokeImpl(const ParamPack& params)
        {
            (*Function)(params[0].GetValue<Arg0>(), params[1].GetValue<Arg1>());
        }

        template <typename _, typename Arg0, typename Arg1, typename Arg2>
        void InvokeImpl(const ParamPack& params)
        {
            (*Function)(params[0].GetValue<Arg0>(), params[1].GetValue<Arg1>(), params[2].GetValue<Arg2>());
        }

    private:
        Func Function;
    };
}