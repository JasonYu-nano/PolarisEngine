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

void StructUnit::GenerateCode(CodeWriter& writer) const
{
    if (writer.Empty())
    {
        writer.WriteLine(GetGeneratedHead());
    }

    writer.WriteEmptyRow();
    writer.WriteLine(String::Format(R"(#define META_STRUCT_GENERATED_{}() \)", RecordName));
    writer.AddTab();
    writer.WriteLine("\\");

    writer.WriteLine(R"(IMPL_META_STRUCT_GENERATED_START() \)");

    writer.AddTab();

    if (SuperName.Empty())
    {
        writer.WriteLine(String::Format(R"(DECLARE_STRUCT_START("{0}", {1}, {2}) \)", RecordName, "nullptr", MetaFlagsToString()));
    }
    else
    {
        writer.WriteLine(String::Format(R"(DECLARE_STRUCT_START("{0}", {1}::MetaObject(), {2}) \)", RecordName, SuperName, MetaFlagsToString()));
    }

    writer.WriteLine(R"(DEFINE_STRUCT_META_DATA({}) \)");

    writer.AddTab();

    for (auto&& property: Properties)
    {
        property.GenerateMetaProperty(writer);
    }

    writer.RemoveTab();

    writer.WriteLine(String::Format(R"(DECLARE_STRUCT_END() \)"));
    writer.RemoveTab();

    writer.WriteLine("IMPL_META_STRUCT_GENERATED_END()");
    writer.RemoveTab();

    writer.WriteEmptyRow();

    if (!NameSpace.Empty())
    {
        writer.WriteLine(String::Format("namespace {}", NameSpace));
        writer.WriteLine("{");
        writer.AddTab();
    }

    writer.WriteLine(String::Format(R"(static DeferInitializer _register{0}(&RegisterMetaObject<struct {0}>);)", RecordName));

    if (!NameSpace.Empty())
    {
        writer.RemoveTab();
        writer.WriteLine("}");
    }
}
