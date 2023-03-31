#include "property_unit.hpp"
#include "parse_utils.hpp"

bool PropertyUnit::IsValidCursor(const CXCursor& cursor)
{
    if (!ParseUtils::CursorHasMeta(cursor))
    {
        return false;
    }

    CX_TOSTRING_EXPR(type, clang_getTypeSpelling(clang_getCursorType(cursor)));
    ParseUtils::PrettyTypeName(type);

    if (!ParseUtils::IsValidMetaType(type))
    {
        return false;
    }

    return true;
}

void PropertyUnit::Parse()
{
    Super::Parse();

    CX_ASSIGN_STRING_EXPR(PropertyName, clang_getCursorSpelling(MyCursor));
    ParseUtils::PrettyTypeName(PropertyName);

    CX_ASSIGN_STRING_EXPR(Type, clang_getTypeSpelling(clang_getCursorType(MyCursor)));
    ParseUtils::PrettyTypeName(Type);
}

void PropertyUnit::GenerateMetaProperty(CodeWriter& writer) const
{
    if (OwnerRecord->GetRecordType() == ERecordType::Class)
    {
        writer.WriteLine(String::Format(R"(DECLARE_CLASS_PROPERTY_START("{0}", OFFSET_OF({1}, {0}), {2}) \)", PropertyName, OwnerRecord->GetRecordName(), MetaFlagsToString()));
        writer.WriteLine(String::Format(R"(DEFINE_PROPERTY_META_DATA({}) \)", MetaDataToString()));
        writer.WriteLine(R"(DECLARE_CLASS_PROPERTY_END() \)");
    }
    else if (OwnerRecord->GetRecordType() == ERecordType::Struct)
    {
        writer.WriteLine(String::Format(R"(DECLARE_STRUCT_PROPERTY_START("{0}", OFFSET_OF({1}, {0}), {2}) \)", PropertyName, OwnerRecord->GetRecordName(), MetaFlagsToString()));
        writer.WriteLine(String::Format(R"(DEFINE_PROPERTY_META_DATA({}) \)", MetaDataToString()));
        writer.WriteLine(R"(DECLARE_STRUCT_PROPERTY_END() \)");
    }
}
