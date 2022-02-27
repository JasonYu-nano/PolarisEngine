#pragma once

#include "reflection/set_type.hpp"
#include "reflection/param.hpp"

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

    class IMethodInst
    {
    public:
        virtual Variant Invoke(Variant obj) = 0;

        virtual Variant Invoke(Variant obj, const ParamPack& params) = 0;

        virtual int32 GetParamCount() const = 0;
    };

    template <typename ClassType, typename RetType, typename... Args>
    class MethodInst : public IMethodInst
    {
        typedef RetType(ClassType::* Func)(Args...);
        typedef RetType(ClassType::* ConstFunc)(Args...) const;

    public:
        MethodInst(Func func) : Instance(reinterpret_cast<ConstFunc>(func)) {}

        MethodInst(ConstFunc func) : Instance(func) {}

        Variant Invoke(Variant obj) final
        {
            if constexpr(sizeof...(Args) == 0)
            {
                auto&& val = (obj.GetValue<ClassType>().*Instance)();
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
            auto&& ret = (obj.GetValue<ClassType>().*Instance)();
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
            auto&& ret = (obj.GetValue<ClassType>().*Instance)(params[0].GetValue<Arg0>());
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
        template <typename Arg>
        Variant InvokeImpl(Arg param)
        {
            return Variant(Instance(param));
        }

    private:
        ConstFunc Instance;
    };

    class Method : public MetaObject
    {
    public:
        Method(IMethodInst* inst) : Instance(inst) {}

        Variant Invoke(const Variant& obj)
        {
            auto ret = Instance->Invoke(obj);
            return ret;
        }

        Variant Invoke(const Variant& obj, const ParamPack& params)
        {
            int32 argCount = params.GetCount();
            if (argCount >= Instance->GetParamCount())
            {
                return Instance->Invoke(obj, params);
            }
            else
            {
                ParamPack& mutableParams = const_cast<ParamPack&>(params);
                for (int32 idx = argCount; idx < Params.GetCount(); ++idx)
                {
                    if (Params[idx].HasDefault())
                    {
                        mutableParams[idx] = Params[idx].GetDefault();
                    }
                    else
                    {
                        PL_ERROR("Engine", _T("Param of method don't has default value"));
                        break;
                    }
                }

                if (params.GetCount() >= Instance->GetParamCount())
                {
                    return Instance->Invoke(obj, mutableParams);
                }
                return Variant();
            }
        }

    private:
        IMethodInst* Instance;
        DynamicArray<Param> Params;
    };
}