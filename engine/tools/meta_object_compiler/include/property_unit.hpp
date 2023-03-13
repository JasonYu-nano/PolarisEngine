#pragma once

#include "meta_unit.hpp"

namespace Engine
{
    class PropertyUnit : public MetaUnit
    {
        using Super = MetaUnit;

    public:
        PropertyUnit(const CXCursor& cursor, const String& path) : Super(cursor, path) {}

        void Parse() final;

        void GenerateMetaData() override
        {
        }

    private:
        Set<String> GetSupportFlags() override;

        Set<String> GetSupportAttributeNames() override;

    private:
        String Type{ "" };
        String Name{ "" };
        int64 Offset{ 0 };
    };
}
