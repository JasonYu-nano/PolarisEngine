//#include "precompiled_core.hpp"
#include "file_system/path.hpp"

namespace Engine
{
    String Path::Combine(const String& dest, const String& part)
    {
        String path = dest;
        String normalizedPart = part;

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

    String Path::GetExtension(const String& path)
    {
        int32 pos;
        if ((pos = path.LastIndexOf('.')) >= 0 && pos < path.Length() - 1)
        {
            return path.Slices(pos, path.Length() - pos);
        }
        return String();
    }

    String Path::RemoveExtension(const String& path)
    {
        int32 idx = path.LastIndexOf('.');
        if (idx == 0)
        {
            return String();
        }
        else if (idx > 0)
        {
            return path.Slices(0, idx);
        }
        return path;
    }

    String Path::GetShortName(const String& path, bool withExtension)
    {
        String ret = path;
        int32 idx = path.LastIndexOfAny("/\\");
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

    DynamicArray<String> Path::SplitPath(const String& path)
    {
        return path.SplitAny("/\\");
    }

    String Path::Normalize(const String& path)
    {
        String ret = path;
        ret.Replace("\\", "/");

        if (ret.EndsWith('/') && ret.EndsWith("//") && ret.EndsWith(":/"))
        {
            ret.Remove(ret.Length() - 1, 1);
        }

        return ret;
    }
}
