#pragma once

#include "parse_unit.hpp"
#include "meta_data_types.hpp"

namespace Engine
{
    enum class EEnumDeclareType : uint8
    {
        Invalid,
        Enum,
        EnumClass
    };

    class EnumConstantUnit : public ParseUnit
    {
        using Super = ParseUnit;

    public:
        EnumConstantUnit(class EnumUnit* owner, const CXCursor& cursor)
            : OwnerEnum(owner)
            , Super(cursor)
        {}

        void Parse() override;

        void GenerateCode(CodeWriter& writer) const;

        Set<String> GetSupportFlags() override
        {
            return Set<String>();
        }

    private:
        class EnumUnit* OwnerEnum;
        String ConstantName;
        int64 ConstantValue{ 0 };
    };

    class EnumUnit : public RecordUnit
    {
        using Super = RecordUnit;
    public:
        EnumUnit(CXCursor cursor, const String& path, const String& nameSpace) : Super(cursor, path, nameSpace)
        {
            RecordType = ERecordType::Enum;
        }

        void Parse() final;

        void GenerateCode(CodeWriter& headerWriter, CodeWriter& sourceWriter) const override;

    private:
        void CheckEnumDeclareType();

        Set<String> GetSupportFlags() override
        {
            return Set<String>();
        }

    private:
        EEnumDeclareType DeclareType;
        String UnderlyingType;
        Array<EnumConstantUnit> EnumConstants;
    };
}
