#pragma once

#include "parse_unit.hpp"

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

    class MethodUnit : public ParseUnit
    {
        using Super = ParseUnit;

    public:
        static bool IsValidCursor(class ClassUnit* owner, const CXCursor& cursor);

        MethodUnit(class ClassUnit* owner, const CXCursor& cursor)
            : OwnerRecord(owner)
            , Super(cursor)
        {}

        void Parse() final;

        String GetMethodName() const { return MethodName; }

        void GenerateProxyMethodDeclare(CodeWriter& writer) const;

        void GenerateProxyMethodDefine(CodeWriter& writer) const;

        void GenerateMetaMethod(CodeWriter& writer) const;

    private:
        Set<String> GetSupportFlags() override;

    private:
        class ClassUnit* OwnerRecord;
        bool IsStatic{ false };
        String MethodName{ "" };
        ReturnTypeDel ReturnType;
        Array<ArgumentDecl> Args;
    };
}
