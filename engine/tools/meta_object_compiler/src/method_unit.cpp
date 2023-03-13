#include "method_unit.hpp"
#include "parse_utils.hpp"

void MethodUnit::Parse()
{
    Super::Parse();

    CX_ASSIGN_STRING_EXPR(Name, clang_getCursorSpelling(MyCursor));
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

Set<String> MethodUnit::GetSupportFlags()
{
    return Set<String>();
}

Set<String> MethodUnit::GetSupportAttributeNames()
{
    return Set<String>();
}
