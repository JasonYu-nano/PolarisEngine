#pragma once

#include "meta_unit.hpp"

namespace Engine
{
    struct EnumField
    {
        String Name;
        int64 Value;
        Set<String> Flags;
        Map<String, String> Attributes;
    };

    class EnumUnit : public MetaUnit
    {
        using Super = MetaUnit;
    public:
        EnumUnit(const CXCursor& cursor, const String& path) : Super(cursor, path) {}

        void Parse() final;

        void GenerateMetaData() override
        {
        }

    private:
        Set<String> GetSupportFlags() override;

        Set<String> GetSupportAttributeNames() override;

    private:
        Array<EnumField> Fields;
    };
}
