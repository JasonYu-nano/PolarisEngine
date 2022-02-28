#pragma once

#include "reflection/set_type.hpp"
#include "reflection/param.hpp"
#include "reflection/details/method_invoker.hpp"

namespace Engine
{
    class Method : public MetaObject
    {
    public:
        Method(IMethodInvoker* inst) : Invoker(inst) {}

        Variant Invoke(const Variant& obj)
        {
            return Invoker->Invoke(obj);
        }

        Variant Invoke(const Variant& obj, const ParamPack& params)
        {
            int32 argCount = params.GetCount();
            if (argCount >= Invoker->GetParamCount())
            {
                return Invoker->Invoke(obj, params);
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

                if (params.GetCount() == Invoker->GetParamCount())
                {
                    return Invoker->Invoke(obj, mutableParams);
                }
                return {};
            }
        }

    private:
        IMethodInvoker* Invoker;
        DynamicArray<Param> Params;
    };
}