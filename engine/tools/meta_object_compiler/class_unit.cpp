#include "class_unit.hpp"
#include "parse_utils.hpp"

namespace Engine
{
    void ClassUnit::Parse()
    {
        Super::Parse();

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
                    if (MethodUnit::IsValidCursor(this, child))
                    {
                        MethodUnit method(this, child);
                        method.Parse();
                        Methods.Add(std::move(method));
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

    void ClassUnit::GenerateCode(CodeWriter& headerWriter, CodeWriter& sourceWriter) const
    {
        if (headerWriter.Empty())
        {
            headerWriter.WriteLine(GetGeneratedHead());
        }

        headerWriter.WriteEmptyLine();
        headerWriter.WriteLine(String::Format(R"(#define MOC_GENERATED_{}() \)", RecordName));
        headerWriter.AddTab();

        for (auto&& method: Methods)
        {
            method.GenerateProxyMethod(headerWriter);
        }

        if (!Methods.Empty())
        {
            headerWriter.WriteLine("\\");
        }

        headerWriter.WriteLine(R"(IMPL_META_CLASS_GENERATED_START() \)");

        headerWriter.AddTab();

        if (SuperName.Empty())
        {
            headerWriter.WriteLine(String::Format(R"(DECLARE_CLASS_START("{0}", {1}, {2}) \)", RecordName, "nullptr", MetaFlagsToString()));
        }
        else
        {
            headerWriter.WriteLine(String::Format(R"(DECLARE_CLASS_START("{0}", {1}::MetaObject(), {2}) \)", RecordName, SuperName, MetaFlagsToString()));
        }

        headerWriter.WriteLine(String::Format(R"(DEFINE_CLASS_META_DATA({}) \)", MetaDataToString()));

        headerWriter.AddTab();

        for (auto&& property: Properties)
        {
            property.GenerateMetaProperty(headerWriter);
        }

        for (auto&& method: Methods)
        {
            method.GenerateMetaMethod(headerWriter);
        }

        headerWriter.RemoveTab();

        headerWriter.WriteLine(String::Format(R"(DECLARE_CLASS_END() \)"));
        headerWriter.RemoveTab();

        headerWriter.WriteLine("IMPL_META_CLASS_GENERATED_END()");
        headerWriter.RemoveTab();

        headerWriter.WriteEmptyLine();

        if (!NameSpace.Empty())
        {
            headerWriter.WriteLine(String::Format("namespace {}", NameSpace));
            headerWriter.WriteLine("{");
            headerWriter.AddTab();
        }

        headerWriter.WriteLine(String::Format(R"(static DeferInitializer _register{0}(&RegisterMetaObject<class {0}>);)", RecordName));

        if (!NameSpace.Empty())
        {
            headerWriter.RemoveTab();
            headerWriter.WriteLine("}");
        }
    }

    bool ClassUnit::HasMethod(const String& methodName) const
    {
        int32 index = Methods.Find([&methodName](const MethodUnit& unit){
           return unit.GetMethodName() == methodName;
        });
        return index != INDEX_NONE;
    }

    Set<String> ClassUnit::GetSupportFlags()
    {
        return Set<String>();
    }
}