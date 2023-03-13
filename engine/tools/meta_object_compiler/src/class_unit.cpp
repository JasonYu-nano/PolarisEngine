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
                    if (ParseUtils::CursorHasMeta(child) && ParseUtils::IsValidPropertyType(child))
                    {
                        PropertyUnit unit(child, Path);
                        unit.Parse();
                        Properties.Add(std::move(unit));
                        break;
                    }
                }
                case CXCursor_CXXMethod:
                    if (ParseUtils::CursorHasMeta(child))
                    {
                        MethodUnit unit(child, Path);
                        unit.Parse();
                        Methods.Add(std::move(unit));
                        break;
                    }
                default:
                {

                }
            }
        }
    }

    Set<String> ClassUnit::GetSupportFlags()
    {
        return Set<String>();
    }

    Set<String> ClassUnit::GetSupportAttributeNames()
    {
        return Set<String>();
    }
}