//#include "precompiled_core.hpp"
#include "file_system/path.hpp"

namespace Engine
{
    UString Path::MakePath(const UString& dest, const UString& part)
    {
        UString path = dest;
        UString normalizedPart = part;

        if (dest.EndsWith("/") || dest.EndsWith("\\"))
        {
            if (part.StartsWith("/"))
            {
                normalizedPart.Remove(0, 1);
            }
            else if (part.StartsWith("\\"))
            {
                normalizedPart.Remove(0, 2);
            }
        }
        else
        {
            if (!part.StartsWith("/") && !part.StartsWith("\\"))
            {
                normalizedPart.Prepend("/");
            }
        }

        path.Append(normalizedPart);
        return path;
    }

    UString Path::GetExtension(const UString& path)
    {
        int32 pos;
        if ((pos = path.LastIndexOf('.')) >= 0 && pos < path.Length() - 1)
        {
            return path.Slices(pos, path.Length() - pos);
        }
        return UString();
    }

    UString Path::RemoveExtension(const UString& path)
    {
        strsize idx = path.LastIndexOf('.');
        if (idx == 0)
        {
            return UString();
        }
        else if (idx > 0)
        {
            return path.Slices(0, idx);
        }
        return path;
    }

    UString Path::GetShortName(const UString& path, bool withExtension)
    {
        UString ret = path;
        strsize idx = path.LastIndexOfAny("/\\");
        if (idx > -1)
        {
            ret = ret.Slices(idx + 1, path.Length() - idx - 1);
        }

        if (!withExtension)
        {
            return RemoveExtension(ret);
        }

        return ret;
    }

    DynamicArray<UString> Path::SplitPath(const UString& path)
    {
        return path.SplitAny("/\\");
    }
}
