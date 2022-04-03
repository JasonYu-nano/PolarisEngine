#pragma once

#include "clang-c/Index.h"
#include "core_minimal_public.hpp"

namespace Engine
{
    class Cursor
    {
    public:
        explicit Cursor(CXCursor cursor);

        bool HasProperty() const;

        DynamicArray<Cursor> GetChildren() const;

        CXCursorKind GetKind() const;

        String GetDisplayName() const;
    private:
        CXCursor RawCursor;
    };
}