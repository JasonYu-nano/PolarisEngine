#pragma once

#include "reflection/set_type.hpp"
#include "param.hpp"

namespace Engine
{
    class IMethodInst
    {
    public:
        virtual Variant Invoke(Variant obj) = 0;

        virtual Variant Invoke(const DynamicArray<Variant>& params) = 0;

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
            auto val = (obj.GetValue<ClassType>().*Instance)();
            auto ret = Variant(val);
            return ret;
        }

        Variant Invoke(const DynamicArray<Variant>& params) final
        {
            if (params.GetCount() != sizeof...(Args))
            {
                PL_WARN("Engine", _T("Argument count not match the method"));
                return Variant();
            }
            return Variant();
        }

        int32 GetParamCount() const final
        {
            return sizeof...(Args);
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

        Variant Invoke(const DynamicArray<Variant>& params)
        {
            int32 argCount = params.GetCount();
            if (argCount >= Instance->GetParamCount())
            {
                return Instance->Invoke(params);
            }
            else
            {
                DynamicArray<Variant> packedParams(Instance->GetParamCount());
                packedParams = params;
                for (int32 idx = argCount; idx < Params.GetCount(); ++idx)
                {
                    if (Params[idx].HasDefault())
                    {
                        packedParams.Add(Params[idx].GetDefault());
                    }
                    else
                    {
                        PL_ERROR("Engine", _T("Param of method don't has default value"));
                        break;
                    }
                }

                if (packedParams.GetCount() >= Instance->GetParamCount())
                {
                    return Instance->Invoke(packedParams);
                }
                return Variant();
            }
        }

    private:
        IMethodInst* Instance;
        DynamicArray<Param> Params;
    };
}