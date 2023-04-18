#include "parse_unit.hpp"
#include "parse_utils.hpp"
#include "file_system/path.hpp"

namespace Engine
{
    void ParseUnit::Parse()
    {
        ParseMetaData(MyCursor, Flags, MetaData);
    }

    void ParseUnit::ParseMetaData(CXCursor cursor, Set<String>& flags, Map<String, String>& metaDataMap)
    {
        String unparseAttributes = ParseUtils::GetUserDeclareAttribute(cursor);
        if (unparseAttributes.Empty())
        {
            return;
        }

        Set<String> supportFlags = GetSupportFlags();

        Array<String> groups = unparseAttributes.Split(',', SkipEmptyParts);
        for (auto&& group : groups)
        {
            if (!group.Contains('='))
            {
                String flag = group.Trimmed();
                if (!flag.Empty() && supportFlags.Contains(flag))
                {
                    flags.Add(std::move(flag));
                }
            }
            else
            {
                Array<String> values = group.Split('=');
                if (values.Size() == 2)
                {
                    String key = values[0].Trimmed();
                    String value = values[1].Trimmed();
                    value.Remove('"');
                    if (!key.Empty() && !value.Empty())
                    {
                        metaDataMap.Add(std::move(key), std::move(value));
                    }
                }
            }
        }
    }

    String ParseUnit::MetaFlagsToString() const
    {
        if (Flags.Empty())
        {
            return "MF_None";
        }

        String ret;
        bool isFirstFlag = true;
        for (auto&& flag : Flags)
        {
            if (isFirstFlag)
            {
                ret.Append(String::Format("MF_{}", flag));
                isFirstFlag = false;
            }
            else
            {
                ret.Append('|').Append(String::Format("MF_{}", flag));
            }
        }

        return ret;
    }

    String ParseUnit::MetaDataToString() const
    {
        String ret = '{';

        bool isFirstMapElement = true;
        for (auto&& pair : MetaData)
        {
            if (isFirstMapElement)
            {
                ret.Append(String::Format("{{\"{0}\", \"{1}\"}}", pair.Key, pair.Value));
                isFirstMapElement = false;
            }
            else
            {
                ret.Append(String::Format(", {{\"{0}\", \"{1}\"}}", pair.Key, pair.Value));
            }
        }
        ret.Append('}');
        return ret;
    }

    void RecordUnit::Parse()
    {
        Super::Parse();

        CX_ASSIGN_STRING_EXPR(RecordName, clang_getCursorSpelling(MyCursor));
        ParseUtils::PrettyTypeName(RecordName);

        CXSourceLocation location = clang_getCursorLocation(MyCursor);
        CXFile file;
        uint32 line = 0;
        clang_getExpansionLocation(location, &file, &line, nullptr, nullptr);

        CX_ASSIGN_STRING_EXPR(Path, clang_getFileName(file));
        Path::Normalize(Path);

        FileID = String::Format("{}_{}", GetFilePathMacro(), line + 2);
        OwnerBuildTarget = StringID(ParseUtils::GetFileOwnerBuildTarget(Path));
    }

    String RecordUnit::GetGeneratedHead() const
    {
        return String::Format(R"(/**
 * This code was generated by MetaObjectCompiler.
 * DO NOT modify this manually!
 */

#pragma once

#include "object_register.hpp"
#include "meta_macro.hpp"
#include "foundation/smart_ptr.hpp"

#undef FILE_PATH
#define FILE_PATH {})", GetFilePathMacro());
    }

    String RecordUnit::GetGeneratedSource() const
    {
        return R"(/**
 * This code was generated by MetaObjectCompiler.
 * DO NOT modify this manually!
 */

#include "meta_object.hpp"
#include "meta_construct_utils.hpp"
)";
    }

    String RecordUnit::GetFilePathMacro() const
    {
        String macro = Path;
        macro.Remove(':');
        macro.Replace('/', '_');
        macro.Replace('.', '_');
        macro.ToUpperLatin1();
        return macro;
    }
}