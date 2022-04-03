#pragma once

#include "core_minimal_public.hpp"

namespace Engine
{
    class ReflectionUnit
    {
    public:
        static UniquePtr<ReflectionUnit> CreateUnit(const String& filePath, DynamicArray<const ansi*> args);

    public:
        String FilePath;
    };
}