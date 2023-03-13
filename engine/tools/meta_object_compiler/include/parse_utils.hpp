#pragma once

#include "clang-c/Index.h"
#include "foundation/string.hpp"

#define CX_TOSTRING(cxstring, stringName) \
    String stringName = clang_getCString(cxstring); \
    clang_disposeString(cxstring);

#define CX_ASSIGN_STRING(cxstring, stringName) \
    stringName = clang_getCString(cxstring); \
    clang_disposeString(cxstring);

#define CX_ASSIGN_STRING_EXPR(string, expr) \
    { \
        CXString _cxstr = expr;\
        string = clang_getCString(_cxstr); \
        clang_disposeString(_cxstr); \
    }

namespace Engine
{
    class ParseUtils
    {
    public:
        static Array<CXCursor> GetCursorChildren(CXCursor parent);

        static bool CursorHasMeta(CXCursor cursor);

        static String GetUserDeclareAttribute(CXCursor cursor);

        static bool IsValidPropertyType(CXCursor cursor);

        /**
         * Remove type reference, engine namespace, convert to engine typedef.
         * @param type
         */
        NODISCARD static String PrettyTypeName(const String& type);

        /**
         * Remove type reference, engine namespace, convert to engine typedef.
         * @param type
         */
        static void PrettyTypeName(String& type);

        static bool IsLeftReference(const String& type);

        static bool IsRightReference(const String& type);
    };
}
