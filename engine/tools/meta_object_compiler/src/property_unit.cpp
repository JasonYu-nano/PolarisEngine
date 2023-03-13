#include "property_unit.hpp"
#include "parse_utils.hpp"

void PropertyUnit::Parse()
{
    Super::Parse();

    CX_ASSIGN_STRING_EXPR(Name, clang_getCursorSpelling(MyCursor));
    int64 offset = clang_Cursor_getOffsetOfField(MyCursor);
    if (offset < 0)
    {

    }
    else
    {
        Offset = offset;
    }

    CX_ASSIGN_STRING_EXPR(Type, clang_getTypeSpelling(clang_getCursorType(MyCursor)));
    ParseUtils::PrettyTypeName(Type);
}

Set<String> PropertyUnit::GetSupportFlags()
{
    return Set<String>();
}

Set<String> PropertyUnit::GetSupportAttributeNames()
{
    return Set<String>();
}
