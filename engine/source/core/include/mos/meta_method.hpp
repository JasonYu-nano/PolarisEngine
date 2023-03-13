#pragma once

#include "memory/memory.hpp"
#include "mos/meta_attributes.hpp"

namespace Engine
{
    struct CORE_API MethodArgument
    {
        bool IsConst = false;
        StringID Name;
    };

    class CORE_API MetaMethod : public MetaAttributes
    {
        typedef void(* Fn)(void* args);
    public:
        void Invoke(void* args)
        {
            (*MethodProxy)(args);
        }

    private:
        Fn MethodProxy;
        bool IsConst;
        StringID Name;
        Array<MethodArgument> Arguments;
    };
}
