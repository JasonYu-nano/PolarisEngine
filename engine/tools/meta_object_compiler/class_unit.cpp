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
        // Write *.hpp
        if (headerWriter.Empty())
        {
            headerWriter.WriteLine(GetGeneratedHead());
        }

        headerWriter.WriteEmptyLine();
        headerWriter.WriteLine(String::Format(R"(#define MOC_GENERATED_{}() \)", FileID));
        headerWriter.AddTab();

        for (auto&& method: Methods)
        {
            method.GenerateProxyMethodDeclare(headerWriter);
        }

        if (!Methods.Empty())
        {
            headerWriter.WriteLine("\\");
        }

        if (!SuperName.Empty())
        {
            headerWriter.WriteLine(String::Format(R"(using Super = {}; \)", SuperName));
        }

        headerWriter.WriteLine(R"(IMPL_META_CLASS_GENERATED())");
        headerWriter.RemoveTab();

        // Write *.cpp
        if (sourceWriter.Empty())
        {
            sourceWriter.WriteLine(GetGeneratedSource());
            sourceWriter.WriteLine(String::Format(R"(#include "{}")", GetFilePath()));
        }

        sourceWriter.WriteEmptyLine();

        for (auto&& method: Methods)
        {
            method.GenerateProxyMethodDefine(sourceWriter);
            sourceWriter.WriteEmptyLine();
        }

        sourceWriter.WriteLine(String::Format(R"(DEFINE_CONSTRUCT_META_CLASS_START({}) \)", RecordName));
        sourceWriter.AddTab();
        sourceWriter.WriteLine(String::Format(R"(DEFINE_CLASS("{0}", {1}, {2}) \)", RecordName, (SuperName.Empty() ? "nullptr" : String::Format("{}::MetaObject()", SuperName)), MetaFlagsToString()));
        sourceWriter.WriteLine(String::Format(R"(DEFINE_CLASS_META_DATA({}) \)", MetaDataToString()));

        sourceWriter.AddTab();

        for (auto&& property: Properties)
        {
            property.GenerateMetaProperty(sourceWriter);
        }

        for (auto&& method: Methods)
        {
            method.GenerateMetaMethod(sourceWriter);
        }

        sourceWriter.RemoveTab();

        sourceWriter.WriteLine(String::Format(R"(DEFINE_CONSTRUCT_META_CLASS_END())"));
        sourceWriter.RemoveTab();

        sourceWriter.WriteEmptyLine();

        if (!NameSpace.Empty())
        {
            sourceWriter.WriteLine(String::Format("namespace {}", NameSpace));
            sourceWriter.WriteLine("{");
            sourceWriter.AddTab();
        }

        sourceWriter.WriteLine(String::Format(R"(static DeferInitializer register{0}(&RegisterMetaObject<class {0}>);)", RecordName));

        if (!NameSpace.Empty())
        {
            sourceWriter.RemoveTab();
            sourceWriter.WriteLine("}");
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