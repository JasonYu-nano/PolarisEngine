#pragma once

#include "reflection/details/method_invoker_interface.hpp"

namespace Engine
{
    template <typename ClassType, typename RetType, typename... Args>
    class MethodInvoker : public IMethodInvoker
    {
        typedef RetType(ClassType::* Func)(Args...);
        typedef RetType(ClassType::* ConstFunc)(Args...) const;

    public:
        explicit MethodInvoker(Func func) : Function(reinterpret_cast<ConstFunc>(func)) {}

        explicit MethodInvoker(ConstFunc func) : Function(func) {}

        Variant Invoke(const ParamPack& params) final
        {
            if (params.GetCount() != sizeof...(Args) + 1)
            {
                PL_ERROR("Engine", _T("Argument count not match the method"));
                return {};
            }

            return InvokeImpl<ClassType, Args...>(params);
        }

        int32 GetParamCount() const final
        {
            return sizeof...(Args);
        }

        bool IsStatic() const final
        {
            return false;
        }

    private:
        template <typename ObjType>
        Variant InvokeImpl(const ParamPack& params)
        {
            ObjType& obj = params[0].GetValue<ObjType>();
            auto&& ret = (obj.*Function)();
            INVOKE_RETURN_VARIANT(RetType, ret);
        }

        template <typename ObjType, typename Arg0>
        Variant InvokeImpl(const ParamPack& params)
        {
            ObjType& obj = params[0].GetValue<ObjType>();
            auto&& ret = (obj.*Function)(params[1].GetValue<Arg0>());
            INVOKE_RETURN_VARIANT(RetType, ret);
        }

        template <typename ObjType, typename Arg0, typename Arg1>
        Variant InvokeImpl(const ParamPack& params)
        {
            ObjType& obj = params[0].GetValue<ObjType>();
            auto&& ret = (obj.*Function)(params[1].GetValue<Arg0>(), params[2].GetValue<Arg1>());
            INVOKE_RETURN_VARIANT(RetType, ret);
        }

        template <typename ObjType, typename Arg0, typename Arg1, typename Arg2>
        Variant InvokeImpl(const ParamPack& params)
        {
            ObjType& obj = params[0].GetValue<ObjType>();
            auto&& ret = (obj.*Function)(params[1].GetValue<Arg0>(), params[2].GetValue<Arg1>(), params[3].GetValue<Arg2>());
            INVOKE_RETURN_VARIANT(RetType, ret);
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
        explicit MethodInvoker(Func func) : Function(reinterpret_cast<ConstFunc>(func)) {}

        explicit MethodInvoker(ConstFunc func) : Function(func) {}

        Variant Invoke(const ParamPack& params) final
        {
            if (params.GetCount() != sizeof...(Args) + 1)
            {
                PL_ERROR("Engine", _T("Argument count not match the method"));
                return {};
            }

            InvokeImpl<ClassType, Args...>(params);
            return {};
        }

        int32 GetParamCount() const final
        {
            return sizeof...(Args);
        }

        bool IsStatic() const final
        {
            return false;
        }

    private:
        template <typename ObjType>
        void InvokeImpl(const ParamPack& params)
        {
            ObjType& obj = params[0].GetValue<ObjType>();
            (obj.*Function)();
        }

        template <typename ObjType, typename Arg0>
        void InvokeImpl(const ParamPack& params)
        {
            ObjType& obj = params[0].GetValue<ObjType>();
            (obj.*Function)(params[1].GetValue<Arg0>());
        }

        template <typename ObjType, typename Arg0, typename Arg1>
        void InvokeImpl(const ParamPack& params)
        {
            ObjType& obj = params[0].GetValue<ObjType>();
            (obj.*Function)(params[1].GetValue<Arg0>(), params[2].GetValue<Arg1>());
        }

        template <typename ObjType, typename Arg0, typename Arg1, typename Arg2>
        void InvokeImpl(const ParamPack& params)
        {
            ObjType& obj = params[0].GetValue<ObjType>();
            (obj.*Function)(params[1].GetValue<Arg0>(), params[2].GetValue<Arg1>(), params[3].GetValue<Arg2>());
        }

    private:
        ConstFunc Function;
    };
}