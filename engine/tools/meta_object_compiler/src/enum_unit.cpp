#include "enum_unit.hpp"
#include "attribute_types.hpp"
#include "parse_utils.hpp"

void EnumUnit::Parse()
{
    Super::Parse();

    for (auto&& child : ParseUtils::GetCursorChildren(MyCursor))
    {
        switch (clang_getCursorKind(child))
        {
            case CXCursor_EnumConstantDecl:
            {
                EnumField field;
                CXString spelling = clang_getCursorSpelling(child);
                CX_TOSTRING(spelling, name);
                field.Name = name;

                field.Value = clang_getEnumConstantDeclValue(child);

                ParseMetaAttribute(child, field.Flags, field.Attributes);

                Fields.Add(std::move(field));
                break;
            }
            default:
            {

            }
        }
    }
}

Set<String> EnumUnit::GetSupportFlags()
{
    return Set<String>();
}

Set<String> EnumUnit::GetSupportAttributeNames()
{
    return {
        AttributeName::AN_DisplayName,
        AttributeName::AN_Tooltips
    };
}
