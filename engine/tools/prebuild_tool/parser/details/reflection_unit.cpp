#include "parser/reflection_unit.hpp"
#include "parser/cursor.hpp"

namespace Engine
{
    UniquePtr<ReflectionUnit> ReflectionUnit::CreateUnit(const String& filePath, DynamicArray<const ansi*> args)
    {
        CXIndex cxIndex = clang_createIndex(true, false);
        CXTranslationUnit translationUnit;

        auto error = clang_parseTranslationUnit2(
            cxIndex,
            (const char*)filePath,
            args.GetData(),
            static_cast<int32>(args.GetCount()),
            nullptr,
            0,
            CXTranslationUnit_None,
            &translationUnit
        );

        Cursor cursor(clang_getTranslationUnitCursor(translationUnit));

        UniquePtr<ReflectionUnit> unit = nullptr;

        for (auto&& child : cursor.GetChildren())
        {
            CXCursorKind kind = child.GetKind();
            switch (child.GetKind())
            {
                case CXCursor_EnumDecl:
                {
                    if (child.HasProperty())
                    {
                        if (unit == nullptr)
                        {
                            unit = MakeUniquePtr<ReflectionUnit>();
                            unit->FilePath = filePath;
                        }
                        // TODO: Support enum
                    }
                    break;
                }
                case CXCursor_StructDecl:
                case CXCursor_ClassDecl:
                {
                    if (child.HasProperty())
                    {
                        auto name = child.GetDisplayName();
                        if (unit == nullptr)
                        {
                            unit = MakeUniquePtr<ReflectionUnit>();
                            unit->FilePath = filePath;
                        }
                        // TODO: Construct structure
                    }
                    break;
                }
                default:
                {
                    // do nothing
                }
            }
        }

        return unit;
    }
}