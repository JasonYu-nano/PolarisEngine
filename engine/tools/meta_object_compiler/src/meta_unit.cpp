#include "meta_unit.hpp"
#include "parse_utils.hpp"

namespace Engine
{
    void MetaUnit::Parse()
    {
        ParseMetaAttribute(MyCursor, Flags, Attributes);
    }

    void MetaUnit::ParseMetaAttribute(const CXCursor& cursor, Set<String>& flags, Map<String, String>& attributes)
    {
        String unparseAttributes = ParseUtils::GetUserDeclareAttribute(cursor);
        if (unparseAttributes.Empty())
        {
            return;
        }

        Set<String> supportFlags = GetSupportFlags();
        Set<String> supportAttributes = GetSupportAttributeNames();

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
                    if (!key.Empty() && !value.Empty() && supportAttributes.Contains(key))
                    {
                        attributes.Add(std::move(key), std::move(value));
                    }
                }
            }
        }
    }
}