#pragma once

#include "memory/memory.hpp"
#include "meta_attributes.hpp"

namespace Engine
{
    struct OBJECT_API MethodArgument
    {
        bool IsConst = false;
        StringID Name;
    };

    class OBJECT_API MetaMethod : public MetaObject
    {
    public:
        typedef void(* FnPtr)(void* args);

        MetaMethod(FnPtr proxy, StringID fnName, EMetaFlag flags)
            : MethodProxy(proxy)
            , Name(fnName)
        {
            Flags = flags;
        }

        StringID GetMethodName() const { return Name; }

        void Invoke(void* args)
        {
            (*MethodProxy)(args);
        }

    private:
        FnPtr MethodProxy;
        StringID Name;
        Array<MethodArgument> Arguments;
    };
}
