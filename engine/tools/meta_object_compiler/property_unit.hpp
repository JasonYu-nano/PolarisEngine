#pragma once

#include "parse_unit.hpp"
#include "meta_data_types.hpp"

namespace Engine
{
    class PropertyUnit : public ParseUnit
    {
        using Super = ParseUnit;

    public:
        static bool IsValidCursor(const CXCursor& cursor);

        PropertyUnit(RecordUnit* owner, const CXCursor& cursor)
            : OwnerRecord(owner)
            , Super(cursor)
        {}

        void Parse() final;

        void GenerateMetaProperty(CodeWriter& writer) const;

    private:
        Set<String> GetSupportFlags() override
        {
            return { MetaFlagName::MF_ReadOnly, MetaFlagName::MF_ReadWrite };
        }

    private:
        RecordUnit* OwnerRecord;
        String Type{ "" };
        String PropertyName{ "" };
    };
}
