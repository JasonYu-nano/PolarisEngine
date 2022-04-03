#include "parser/cursor.hpp"

namespace Engine
{

    Cursor::Cursor(CXCursor cursor) :
        RawCursor(cursor)
    {
    }

    bool Cursor::HasProperty() const
    {
        if (clang_Cursor_hasAttrs(RawCursor) <= 0)
        {
            return false;
        }

        bool result = false;
        auto visitor = [](CXCursor cursor, CXCursor parent, CXClientData data) {
            if (clang_getCursorKind(cursor) == CXCursor_LastPreprocessing)
            {
                return CXChildVisit_Break;
            }

            if (clang_getCursorKind(cursor) == CXCursor_AnnotateAttr)
            {
                bool* result = static_cast<bool*>(data);
                *result = true;
                return CXChildVisit_Break;
            }

            return CXChildVisit_Continue;
        };
        clang_visitChildren(RawCursor, visitor, &result);
        return result;
    }

    DynamicArray<Cursor> Cursor::GetChildren() const
    {
        DynamicArray<Cursor> children;

        auto visitor = [](CXCursor cursor, CXCursor parent, CXClientData data)
        {
            if (clang_getCursorKind(cursor) == CXCursor_LastPreprocessing)
                return CXChildVisit_Break;

            DynamicArray<Cursor>* container = static_cast<DynamicArray<Cursor>*>(data);
            container->Add(Cursor(cursor));

            return CXChildVisit_Continue;
        };

        clang_visitChildren(RawCursor, visitor, &children);

        return children;
    }

    CXCursorKind Cursor::GetKind() const
    {
        return clang_getCursorKind(RawCursor);
    }

    String Cursor::GetDisplayName() const
    {
        CXString cStr = clang_getCursorDisplayName(RawCursor);
#ifdef UNICODE
        String str = A2W(clang_getCString(cStr));
#else
        String str = clang_getCString(cStr);
#endif
        clang_disposeString(cStr);
        return str;
    }
}