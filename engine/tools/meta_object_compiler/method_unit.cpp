#include "method_unit.hpp"
#include "parse_utils.hpp"
#include "class_unit.hpp"
#include "moc_log.hpp"

bool MethodUnit::IsValidCursor(ClassUnit* owner, const CXCursor& cursor)
{
    if (!ParseUtils::CursorHasMeta(cursor))
    {
        return false;
    }

    CX_TOSTRING_EXPR(methodName, clang_getCursorSpelling(cursor));
    if (owner->HasMethod(methodName))
    {
        LOG_WARN(MOC, "Class {} has duplicate meta method name {}", owner->GetRecordName(), methodName);
        return false;
    }

    return true;
}

void MethodUnit::Parse()
{
    Super::Parse();

    CX_ASSIGN_STRING_EXPR(MethodName, clang_getCursorSpelling(MyCursor));
    IsStatic = clang_CXXMethod_isStatic(MyCursor) > 0;

    CXType returnType = clang_getCursorResultType(MyCursor);
    CX_ASSIGN_STRING_EXPR(ReturnType.Type, clang_getTypeSpelling(returnType));
    ReturnType.IsLeftReference = ParseUtils::IsLeftReference(ReturnType.Type);
    ReturnType.IsRightReference = ParseUtils::IsRightReference(ReturnType.Type);

    int32 numArgs = clang_Cursor_getNumArguments(MyCursor);
    if (numArgs > 0)
    {
        for (int32 i = 0; i < numArgs; ++i)
        {
            CXCursor argCursor = clang_Cursor_getArgument(MyCursor, i);
            CXType argType = clang_getCursorType(argCursor);
            ArgumentDecl decl;
            CX_ASSIGN_STRING_EXPR(decl.Name, clang_getCursorSpelling(argCursor));
            CX_ASSIGN_STRING_EXPR(decl.Type, clang_getTypeSpelling(argType));
            decl.IsConst = clang_isConstQualifiedType(argType);
            decl.IsLeftReference = ParseUtils::IsLeftReference(decl.Type);
            decl.IsRightReference = ParseUtils::IsRightReference(decl.Type);

            Args.Add(decl);
        }
    }
}

void MethodUnit::GenerateProxyMethodDeclare(CodeWriter& writer) const
{
    writer.WriteLine(String::Format(R"(DECLARE_PROXY_METHOD({}) \)", MethodName));
}

void MethodUnit::GenerateProxyMethodDefine(CodeWriter& writer) const
{
    writer.WriteLine(String::Format(R"(DEFINE_PROXY_METHOD({}, {}) \)", OwnerRecord->GetRecordName(), MethodName));
    writer.WriteLine(R"({ \)");
    writer.WriteLine(R"(} \)");
}

void MethodUnit::GenerateMetaMethod(CodeWriter& writer) const
{
    writer.WriteLine(String::Format(R"(DEFINE_CLASS_METHOD_START({0}, {1}, {2}) \)", OwnerRecord->GetRecordName(), MethodName, MetaFlagsToString()));
    writer.WriteLine(R"(DEFINE_METHOD_META_DATA({}) \)");
    writer.WriteLine(R"(DEFINE_CLASS_METHOD_END() \)");
}

Set<String> MethodUnit::GetSupportFlags()
{
    return Set<String>();
}
