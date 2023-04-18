#include "struct_unit.hpp"
#include "parse_utils.hpp"
#include "moc_log.hpp"

void StructUnit::Parse()
{
    RecordUnit::Parse();

    for (auto&& child : ParseUtils::GetCursorChildren(MyCursor))
    {
        switch (clang_getCursorKind(child))
        {
            case CXCursor_FieldDecl:
            {
                if (PropertyUnit::IsValidCursor(child))
                {
                    PropertyUnit property(this, child);
                    property.Parse();
                    Properties.Add(std::move(property));
                }
                break;
            }
            case CXCursor_CXXMethod:
            {
                if (ParseUtils::CursorHasMeta(child))
                {
                    CX_TOSTRING_EXPR(methodName, clang_getCursorSpelling(child));
                    LOG_WARN(MOC, "Struct {} contains meta method {}, which is not support by moc", RecordName, methodName);
                }
                break;
            }
            case CXCursor_CXXBaseSpecifier:
            {
                if (SuperName.Empty())
                {
                    CX_ASSIGN_STRING_EXPR(SuperName, clang_getCursorSpelling(child));
                    ParseUtils::PrettyTypeName(SuperName);
                }
                break;
            }
            default:
            {
            }
        }
    }
}

void StructUnit::GenerateCode(CodeWriter& headerWriter, CodeWriter& sourceWriter) const
{
    // Write *.hpp
    if (headerWriter.Empty())
    {
        headerWriter.WriteLine(GetGeneratedHead());
    }

    headerWriter.WriteEmptyLine();
    headerWriter.WriteLine(String::Format(R"(#define MOC_GENERATED_{}() \)", FileID));
    headerWriter.AddTab();

    if (!SuperName.Empty())
    {
        headerWriter.WriteLine(String::Format(R"(using Super = {}; \)", SuperName));
    }

    headerWriter.WriteLine(R"(IMPL_META_STRUCT_GENERATED() )");

    headerWriter.RemoveTab();
    headerWriter.WriteEmptyLine();

    // Write *.cpp
    if (sourceWriter.Empty())
    {
        sourceWriter.WriteLine(GetGeneratedSource());
        sourceWriter.WriteLine(String::Format(R"(#include "{}")", GetFilePath()));
    }

    sourceWriter.WriteEmptyLine();
    sourceWriter.WriteLine(String::Format(R"(DEFINE_CONSTRUCT_META_STRUCT_START({}) \)", RecordName));
    sourceWriter.AddTab();
    sourceWriter.WriteLine(String::Format(R"(DEFINE_STRUCT("{0}", {1}, {2}) \)", RecordName, (SuperName.Empty() ? "nullptr" : String::Format("{}::MetaObject()", SuperName)), MetaFlagsToString()));
    sourceWriter.WriteLine(R"(DEFINE_STRUCT_META_DATA({}) \)");

    sourceWriter.AddTab();
    for (auto&& property: Properties)
    {
        property.GenerateMetaProperty(sourceWriter);
    }

    sourceWriter.RemoveTab();

    sourceWriter.WriteLine(String::Format(R"(DEFINE_CONSTRUCT_META_STRUCT_END())"));
    sourceWriter.RemoveTab();

    if (!NameSpace.Empty())
    {
        sourceWriter.WriteLine(String::Format("namespace {}", NameSpace));
        sourceWriter.WriteLine("{");
        sourceWriter.AddTab();
    }

    sourceWriter.WriteLine(String::Format(R"(static DeferInitializer register{0}(&RegisterMetaObject<struct {0}>);)", RecordName));

    if (!NameSpace.Empty())
    {
        sourceWriter.RemoveTab();
        sourceWriter.WriteLine("}");
    }
}
