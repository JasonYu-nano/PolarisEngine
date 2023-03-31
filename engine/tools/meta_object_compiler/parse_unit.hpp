#pragma once

#include "clang-c/Index.h"
#include "foundation/string.hpp"
#include "foundation/map.hpp"
#include "code_generator_interface.hpp"

namespace Engine
{
    class ParseUnit
    {
    public:
        ParseUnit(CXCursor cursor)
            : MyCursor(cursor)
        {}

        virtual ~ParseUnit() = default;

        virtual void Parse();

    protected:
        void ParseMetaData(CXCursor cursor, Set<String>& flags, Map<String, String>& metaDataMap);

        String MetaFlagsToString() const;

        String MetaDataToString() const;
    private:
        virtual Set<String> GetSupportFlags() = 0;

    protected:
        CXCursor MyCursor;
        Set<String> Flags;
        Map<String, String> MetaData;
    };

    enum class ERecordType : uint8
    {
        Unknown,
        Enum,
        Struct,
        Class
    };

    class RecordUnit : public ParseUnit, public ICodeGenerator
    {
        using Super = ParseUnit;
    public:
        RecordUnit(CXCursor cursor, const String& path, const String& nameSpace)
            : Super(cursor)
            , Path(path)
            , NameSpace(nameSpace)
        {}

        String GetFilePath() const
        {
            return Path;
        }

        String GetRecordName() const { return RecordName; }

        ERecordType GetRecordType() const { return RecordType; }

        void Parse() override;

    protected:
        String GetGeneratedHead() const;

    protected:
        ERecordType RecordType{ ERecordType::Unknown };
        String Path;
        String NameSpace;
        String RecordName;
    };
}
