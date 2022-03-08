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

    class IMethodInvoker
    {
    public:
        virtual Variant Invoke(const ParamPack& params) = 0;

        virtual int32 GetParamCount() const = 0;

        virtual bool IsStatic() const = 0;
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
}