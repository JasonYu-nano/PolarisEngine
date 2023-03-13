#pragma once

#include "meta_unit.hpp"

namespace Engine
{
    struct ReturnTypeDel
    {
        bool IsConst = false;
        bool IsLeftReference = false;
        bool IsRightReference = false;
        String Type = "";
    };

    struct ArgumentDecl
    {
        bool IsConst = false;
        bool IsLeftReference = false;
        bool IsRightReference = false;
        String Type = "";
        String Name = "";
    };

    class MethodUnit : public MetaUnit
    {
        using Super = MetaUnit;

    public:
        MethodUnit(const CXCursor& cursor, const String& path) : Super(cursor, path) {}

        void Parse() final;

        void GenerateMetaData() override
        {
        }

    private:
        Set<String> GetSupportFlags() override;

        Set<String> GetSupportAttributeNames() override;

    private:
        bool IsStatic{ false };
        bool IsConst{ false };
        String Name{ "" };
        ReturnTypeDel ReturnType;
        Array<ArgumentDecl> Args;
    };
}
