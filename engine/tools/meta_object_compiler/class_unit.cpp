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

    void ClassUnit::GenerateCode(CodeWriter& writer) const
    {
        if (writer.Empty())
        {
            writer.WriteLine(GetGeneratedHead());
        }

        writer.WriteEmptyRow();
        writer.WriteLine(String::Format(R"(#define META_CLASS_GENERATED_{}() \)", RecordName));
        writer.AddTab();

        for (auto&& method: Methods)
        {
            method.GenerateProxyMethod(writer);
        }

        if (!Methods.Empty())
        {
            writer.WriteLine("\\");
        }

        writer.WriteLine(R"(IMPL_META_CLASS_GENERATED_START() \)");

        writer.AddTab();

        if (SuperName.Empty())
        {
            writer.WriteLine(String::Format(R"(DECLARE_CLASS_START("{0}", {1}, {2}) \)", RecordName, "nullptr", MetaFlagsToString()));
        }
        else
        {
            writer.WriteLine(String::Format(R"(DECLARE_CLASS_START("{0}", {1}::MetaObject(), {2}) \)", RecordName, SuperName, MetaFlagsToString()));
        }

        writer.WriteLine(String::Format(R"(DEFINE_CLASS_META_DATA({}) \)", MetaDataToString()));

        writer.AddTab();

        for (auto&& property: Properties)
        {
            property.GenerateMetaProperty(writer);
        }

        for (auto&& method: Methods)
        {
            method.GenerateMetaMethod(writer);
        }

        writer.RemoveTab();

        writer.WriteLine(String::Format(R"(DECLARE_CLASS_END() \)"));
        writer.RemoveTab();

        writer.WriteLine("IMPL_META_CLASS_GENERATED_END()");
        writer.RemoveTab();

        writer.WriteEmptyRow();

        if (!NameSpace.Empty())
        {
            writer.WriteLine(String::Format("namespace {}", NameSpace));
            writer.WriteLine("{");
            writer.AddTab();
        }

        writer.WriteLine(String::Format(R"(static DeferInitializer _register{0}(&RegisterMetaObject<class {0}>);)", RecordName));

        if (!NameSpace.Empty())
        {
            writer.RemoveTab();
            writer.WriteLine("}");
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