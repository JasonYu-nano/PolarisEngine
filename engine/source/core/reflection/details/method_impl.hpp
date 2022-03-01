#pragma once

#include "reflection/set_type.hpp"

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

    class IMethodImpl
    {
    public:
        virtual Variant Invoke(Variant obj) = 0;

        virtual Variant Invoke(Variant obj, const ParamPack& params) = 0;

        virtual int32 GetParamCount() const = 0;
    };

    template <typename ClassType, typename RetType, typename... Args>
    class MethodImpl : public IMethodImpl
    {
        typedef RetType(ClassType::* Func)(Args...);
        typedef RetType(ClassType::* ConstFunc)(Args...) const;

    public:
        explicit MethodImpl(Func func) : FuncPtr(reinterpret_cast<ConstFunc>(func)) {}

        explicit MethodImpl(ConstFunc func) : FuncPtr(func) {}

        Variant Invoke(Variant obj) final
        {
            if constexpr(sizeof...(Args) == 0)
            {
                auto&& val = (obj.GetValue<ClassType>().*FuncPtr)();
                if constexpr(std::is_reference_v<RetType>)
                {
                    return Variant(val);
                }
                else
                {
                    return Variant(val, VARIANT_COPY);
                }
            }
            return {};
        }

        Variant Invoke(Variant obj, const ParamPack& params) final
        {
            if (params.GetCount() > sizeof...(Args))
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
            auto&& ret = (obj.GetValue<ClassType>().*FuncPtr)();
            if constexpr(std::is_reference_v<RetType>)
            {
                return Variant(ret);
            }
            else
            {
                return Variant(ret, VARIANT_COPY);
            }
        }

        template <typename _, typename Arg0>
        Variant InvokeImpl(const Variant& obj, const ParamPack& params)
        {
            auto&& ret = (obj.GetValue<ClassType>().*FuncPtr)(params[0].GetValue<Arg0>());
            if constexpr(std::is_reference_v<RetType>)
            {
                return Variant(ret);
            }
            else
            {
                return Variant(ret, VARIANT_COPY);
            }
        }

    private:
        ConstFunc FuncPtr;
    };

    template <typename ClassType, typename... Args>
    class MethodImpl<ClassType, void, Args...> : public IMethodImpl
    {
        typedef void(ClassType::* Func)(Args...);
        typedef void(ClassType::* ConstFunc)(Args...) const;

    public:
        explicit MethodImpl(Func func) : FuncPtr(reinterpret_cast<ConstFunc>(func)) {}

        explicit MethodImpl(ConstFunc func) : FuncPtr(func) {}

        Variant Invoke(Variant obj) final
        {
            if constexpr(sizeof...(Args) == 0)
            {
                (obj.GetValue<ClassType>().*FuncPtr)();
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
            (obj.GetValue<ClassType>().*FuncPtr)();
        }

        template <typename _, typename Arg0>
        void InvokeImpl(const Variant& obj, const ParamPack& params)
        {
            (obj.GetValue<ClassType>().*FuncPtr)(params[0].GetValue<Arg0>());
        }

    private:
        ConstFunc FuncPtr;
    };
}