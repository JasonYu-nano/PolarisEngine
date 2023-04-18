#pragma once

#include "definitions_object.hpp"
#include "foundation/array.hpp"

namespace Engine
{
    class GMetaEnum;

    template <typename T>
    GMetaEnum* GetEnum()
    {
        static_assert(FalseExpr<T>, "Can't match MetaEnum");
        return nullptr;
    }

    typedef void(* FnInitialize)();

    struct OBJECT_API DeferInitializer
    {
        explicit DeferInitializer(FnInitialize fn);
    };

    template <typename Type>
    void RegisterMetaObject()
    {
        Type::MetaObject();
    }

    template <typename Type>
    void RegisterMetaEnum()
    {
        GetEnum<Type>();
    }

namespace Private
{
    inline Array<FnInitialize>& GetDeferInitializeList();
}
}
