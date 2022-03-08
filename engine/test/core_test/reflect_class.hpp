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

        int32 GetNumeric() const;

    private:
        void SetNumeric(int32 value);

        int32 Numeric{ 0 };
    };
}