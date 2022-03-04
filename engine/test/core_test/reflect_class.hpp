#pragma once

#include "core_minimal_public.hpp"
#include "reflect_class.gen.hpp"

namespace Engine
{
    class ReflectClass
    {
        friend class ReflectClassBackend;
    public:
        static void Type()
        {
            ReflectClassBackend::RegisterReflection();
        }

    private:
        int32 GetNumeric() const;

        int32 Numeric{ 0 };
    };
}