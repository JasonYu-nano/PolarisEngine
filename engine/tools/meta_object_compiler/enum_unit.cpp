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

void EnumUnit::GenerateCode(CodeWriter& headerWriter, CodeWriter& sourceWriter) const
{
    if (DeclareType == EEnumDeclareType::Invalid)
    {
        return;
    }

    // Write *.hpp
    if (headerWriter.Empty())
    {
        headerWriter.WriteLine(GetGeneratedHead());
    }

    headerWriter.WriteEmptyLine();

    if (!NameSpace.Empty())
    {
        headerWriter.WriteLine(String::Format("namespace {}", NameSpace));
        headerWriter.WriteLine("{");
        headerWriter.AddTab();
    }

    if (DeclareType == EEnumDeclareType::Enum)
    {
        headerWriter.WriteLine(String::Format("enum {} : {};", RecordName, UnderlyingType));
    }
    else if (DeclareType == EEnumDeclareType::EnumClass)
    {
        headerWriter.WriteLine(String::Format("enum class {} : {};", RecordName, UnderlyingType));
    }

    headerWriter.WriteEmptyLine();
    headerWriter.WriteLine(String::Format("template<> class MetaEnum* Engine::GetEnum<{}>();", RecordName));

    if (!NameSpace.Empty())
    {
        headerWriter.RemoveTab();
        headerWriter.WriteLine("}");
    }

    // Write *.cpp
    if (sourceWriter.Empty())
    {
        sourceWriter.WriteLine(GetGeneratedSource());
        sourceWriter.WriteLine(String::Format(R"(#include "{}")", GetFilePath()));
    }

    sourceWriter.WriteEmptyLine();
    sourceWriter.WriteLine(String::Format(R"(DEFINE_META_ENUM_START({}, {}))", RecordName, MetaFlagsToString()));
    sourceWriter.WriteLine(String::Format(R"(DEFINE_ENUM_META_DATA({}) \)", MetaDataToString()));

    sourceWriter.AddTab();

    for (auto&& constant: EnumConstants)
    {
        constant.GenerateCode(sourceWriter);
    }

    sourceWriter.RemoveTab();


    sourceWriter.WriteLine("DEFINE_META_ENUM_END()");

    sourceWriter.WriteEmptyLine();

    if (!NameSpace.Empty())
    {
        sourceWriter.WriteLine(String::Format("namespace {}", NameSpace));
        sourceWriter.WriteLine("{");
        sourceWriter.AddTab();
    }

    sourceWriter.WriteLine(String::Format("template<> MetaEnum* Engine::GetEnum<{}>()", RecordName));
    sourceWriter.WriteLine("{");
    sourceWriter.AddTab();
    sourceWriter.WriteLine(String::Format("static UniquePtr<MetaEnum> meta = GetMetaEnum{}();", RecordName));
    sourceWriter.WriteLine("return meta.get();");
    sourceWriter.RemoveTab();
    sourceWriter.WriteLine("}");

    sourceWriter.WriteEmptyLine();
    sourceWriter.WriteLine(String::Format(R"(static DeferInitializer register{0}(&RegisterMetaEnum<{0}>);)", RecordName));

    if (!NameSpace.Empty())
    {
        sourceWriter.RemoveTab();
        sourceWriter.WriteLine("}");
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
