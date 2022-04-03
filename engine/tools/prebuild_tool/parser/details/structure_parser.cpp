#include "parser/structure_parser.hpp"

namespace Engine
{

    void StructureParser::Parse(const Cursor& cursor)
    {
        if (!cursor.HasProperty())
        {
            return;
        }

        for (auto&& child : cursor.GetChildren())
        {
            switch (child.GetKind())
            {
                case CXCursor_FieldDecl:
                {
                    if (child.HasProperty())
                    {

                    }
                    break;
                }
                case CXCursor_CXXMethod:
                {
                    if (child.HasProperty())
                    {

                    }
                    break;
                }
                default:
                {
                    // do nothing
                }
            }
        }
    }
}