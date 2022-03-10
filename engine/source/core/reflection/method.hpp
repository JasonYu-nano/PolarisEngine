#pragma once

#include "reflection/set_type.hpp"
#include "reflection/param.hpp"
#include "reflection/details/method_invoker.hpp"
#include "reflection/details/static_method_invoker.hpp"

namespace Engine
{
    class Method : public MetaObject
    {
    public:
        Method() = default;

        explicit Method(IMethodInvoker* inst) : Invoker(inst) {}

        Method(IMethodInvoker* inst, std::initializer_list<Param> params) : Invoker(inst), Params(params) {}

        Variant Invoke(const ParamPack& params)
        {
            int32 argCount = params.GetCount();
            int32 requiredArgCount = Invoker->IsStatic() ? Invoker->GetParamCount() : Invoker->GetParamCount() + 1;

            if (argCount >= requiredArgCount)
            {
                return Invoker->Invoke(params);
            }
            else
            {
                ParamPack& mutableParams = const_cast<ParamPack&>(params);
                for (int32 idx = argCount; idx < requiredArgCount; ++idx)
                {
                    int32 defaultArgIdx = Invoker->IsStatic() ? idx : idx - 1;
                    if (Params[defaultArgIdx].HasDefault())
                    {
                        mutableParams[idx] = Params[defaultArgIdx].GetDefault();
                    }
                    else
                    {
                        PL_ERROR("Engine", _T("Param of method don't has default value"));
                        break;
                    }
                }

                if (params.GetCount() == requiredArgCount)
                {
                    return Invoker->Invoke(mutableParams);
                }
                return {};
            }
        }

        void AddParam(Param&& param)
        {
            Params.Add(param);
        }

    private:
        IMethodInvoker* Invoker{ nullptr };
        DynamicArray<Param> Params;
    };
}