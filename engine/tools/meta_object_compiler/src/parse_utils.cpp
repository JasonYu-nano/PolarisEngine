#include "parse_utils.hpp"

Array<CXCursor> ParseUtils::GetCursorChildren(CXCursor parent)
{
    Array<CXCursor> children;

    auto visitor = [](CXCursor cursor, CXCursor parent, CXClientData data)
    {
        if (clang_getCursorKind(cursor) == CXCursor_LastPreprocessing)
            return CXChildVisit_Break;

        Array<CXCursor>* container = static_cast<Array<CXCursor>*>(data);
        container->Add(cursor);

        return CXChildVisit_Continue;
    };

    clang_visitChildren(parent, visitor, &children);

    return children;
}

bool ParseUtils::CursorHasMeta(CXCursor cursor)
{
    if (clang_Cursor_hasAttrs(cursor) <= 0)
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
    clang_visitChildren(cursor, visitor, &result);
    return result;
}

String ParseUtils::GetUserDeclareAttribute(CXCursor cursor)
{
    if (clang_Cursor_hasAttrs(cursor) <= 0)
    {
        return "";
    }

    CXCursor attrCursor = clang_getNullCursor();
    auto visitor = [](CXCursor cursor, CXCursor parent, CXClientData data) {
        if (clang_getCursorKind(cursor) == CXCursor_LastPreprocessing)
        {
            return CXChildVisit_Break;
        }

        if (clang_getCursorKind(cursor) == CXCursor_AnnotateAttr)
        {
            CXCursor* result = static_cast<CXCursor*>(data);
            *result = cursor;
            return CXChildVisit_Break;
        }

        return CXChildVisit_Continue;
    };
    uint32 visitResult = clang_visitChildren(cursor, visitor, &attrCursor);
    if (!visitResult)
    {
        return "";
    }

    CXString attrString = clang_getCursorDisplayName(attrCursor);
    CX_TOSTRING(attrString, attr);
    return attr;
}

bool ParseUtils::IsValidPropertyType(CXCursor cursor)
{
    return true;
}

String ParseUtils::PrettyTypeName(const String& type)
{
    String prettyType = type;
    if (IsRightReference(type))
    {
        prettyType = prettyType.Chopped(2);
    }
    else if (IsLeftReference(type))
    {
        prettyType = prettyType.Chopped(1);
    }

    prettyType.Remove("Engine::");
    prettyType.Trimmed();
    return prettyType;
}

void ParseUtils::PrettyTypeName(String& type)
{
    if (IsRightReference(type))
    {
        type = type.Chopped(2);
    }
    else if (IsLeftReference(type))
    {
        type = type.Chopped(1);
    }

    type.Remove("Engine::");
    type.Trimmed();
}

bool ParseUtils::IsLeftReference(const String& type)
{
    return !IsRightReference(type) && type.EndsWith("&");
}

bool ParseUtils::IsRightReference(const String& type)
{
    return type.EndsWith("&&");
}
