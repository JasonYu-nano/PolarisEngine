#include "enum_unit.hpp"
#include "parse_utils.hpp"
#include "moc_log.hpp"

void EnumConstantUnit::Parse()
{
    ParseUnit::Parse();

    CX_ASSIGN_STRING_EXPR(ConstantName, clang_getCursorSpelling(MyCursor));
    ConstantValue = clang_getEnumConstantDeclValue(MyCursor);
}

void EnumConstantUnit::GenerateCode(CodeWriter& writer) const
{
    writer.WriteLine(String::Format("DEFINE_META_CONSTANT_START({}, {}, {})", ConstantName, ConstantValue, MetaFlagsToString()));
    writer.WriteLine(String::Format(R"(DEFINE_ENUM_CONSTANT_META_DATA({}) \)", MetaDataToString()));
    writer.WriteLine("DEFINE_META_CONSTANT_END()");
}

void EnumUnit::Parse()
{
    Super::Parse();

    CXType type = clang_getEnumDeclIntegerType(MyCursor);

    CheckEnumDeclareType();

    if (DeclareType == EEnumDeclareType::Invalid)
    {
        return;
    }

    CX_ASSIGN_STRING_EXPR(UnderlyingType, clang_getTypeSpelling(type));
    ParseUtils::PrettyTypeName(UnderlyingType);

    for (auto&& child : ParseUtils::GetCursorChildren(MyCursor))
    {
        switch (clang_getCursorKind(child))
        {
            case CXCursor_EnumConstantDecl:
            {
                EnumConstantUnit unit(this, child);
                unit.Parse();
                EnumConstants.Add(std::move(unit));
                break;
            }
            default:
            {

            }
        }
    }
}

void EnumUnit::GenerateCode(CodeWriter& writer) const
{
    if (DeclareType == EEnumDeclareType::Invalid)
    {
        return;
    }

    if (writer.Empty())
    {
        writer.WriteLine(GetGeneratedHead());
    }

    writer.WriteEmptyRow();

    if (DeclareType == EEnumDeclareType::Enum)
    {
        writer.WriteLine(String::Format("enum {} : {};", RecordName, UnderlyingType));
    }
    else if (DeclareType == EEnumDeclareType::EnumClass)
    {
        writer.WriteLine(String::Format("enum class {} : {};", RecordName, UnderlyingType));
    }

    writer.WriteEmptyRow();
    writer.WriteLine(String::Format(R"(DEFINE_META_ENUM_START({}, {}))", RecordName, MetaFlagsToString()));
    writer.WriteLine(String::Format(R"(DEFINE_ENUM_META_DATA({}) \)", MetaDataToString()));

    writer.AddTab();

    for (auto&& constant: EnumConstants)
    {
        constant.GenerateCode(writer);
    }

    writer.RemoveTab();


    writer.WriteLine("DEFINE_META_ENUM_END()");

    writer.WriteEmptyRow();

    if (!NameSpace.Empty())
    {
        writer.WriteLine(String::Format("namespace {}", NameSpace));
        writer.WriteLine("{");
        writer.AddTab();
    }

    writer.WriteLine(String::Format("template<> MetaEnum* Engine::GetEnum<{}>()", RecordName));
    writer.WriteLine("{");
    writer.AddTab();
    writer.WriteLine(String::Format("static UniquePtr<MetaEnum> meta = GetMetaEnum{}();", RecordName));
    writer.WriteLine("return meta.get();");
    writer.RemoveTab();
    writer.WriteLine("}");

    writer.WriteEmptyRow();

    writer.WriteLine(String::Format(R"(static DeferInitializer _register{0}(&RegisterMetaEnum<{0}>);)", RecordName));

    if (!NameSpace.Empty())
    {
        writer.RemoveTab();
        writer.WriteLine("}");
    }
}

void EnumUnit::CheckEnumDeclareType()
{
    CXSourceRange range = clang_getCursorExtent(MyCursor);
    CXTranslationUnit tu = clang_Cursor_getTranslationUnit(MyCursor);

    CXToken* tokens;
    uint32 numTokens;
    clang_tokenize(tu, range, &tokens, &numTokens);

    bool matchEnum = false;
    bool matchEnumClass = false;
    uint32 index = 0;
    while (index < numTokens)
    {
        CX_TOSTRING_EXPR(spelling, clang_getTokenSpelling(tu, tokens[index]));
        if (spelling != "enum" || index + 1 >= numTokens)
        {
            ++index;
            continue;
        }

        matchEnum = true;
        // not last token && token spelling is enum
        String nextSpelling;
        CX_ASSIGN_STRING_EXPR(nextSpelling, clang_getTokenSpelling(tu, tokens[index + 1]));
        if (nextSpelling == "class")
        {
            matchEnumClass = true;
            break;
        }
        ++index;
    }

    DeclareType = matchEnumClass ? EEnumDeclareType::EnumClass : matchEnum ? EEnumDeclareType::Enum : EEnumDeclareType::Invalid;
    if (DeclareType == EEnumDeclareType::Invalid)
    {
        LOG_ERROR(MOC, R"(Enum '{}' is not a valid expression, please use 'enum META()' or 'enum class META()'")", RecordName);
    }

    clang_disposeTokens(tu, tokens, numTokens);
}
