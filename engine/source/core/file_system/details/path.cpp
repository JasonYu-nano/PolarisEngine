//#include "precompiled_core.hpp"
#include "file_system/path.hpp"

namespace Engine
{
    String Path::MakePath(const String& dest, const String& part)
    {
        String ret = dest;
        if (ret.IsEmpty())
            ret.Append(part);
        else if (ret.EndWith(_T("/")) || ret.EndWith(_T("\\")))
        {
            if (part.StartWith(_T("/")) && part.Length() > 1)
                ret.Append(part.Substr(1, part.Length() - 1));
            else if (part.StartWith(_T("\\")) && part.Length() > 2)
                ret.Append(part.Substr(2, part.Length() - 2));
            else
                ret.Append(part);

        }
        else
        {
            if (part.StartWith(_T("/")) && part.Length() > 1)
                ret.Append(part.Substr(1, part.Length() - 1));
            else if (part.StartWith(_T("\\")) && part.Length() > 2)
                ret.Append(part.Substr(2, part.Length() - 2));
            else
            {
                ret.Append(_T("/"));
                ret.Append(part);
            }
        }
        return ret;
    }

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

    String Path::GetExtension(const String& path)
    {
        size_t pos = path.FindLast(_T("."));
        if (pos != String::InvalidIndex && pos < path.Length() - 1)
        {
            return path.Substr(pos, path.Length() - pos);
        }
        return String::Empty();
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

    String Path::RemoveExtension(const String& path)
    {
        size_t pos = path.FindLast(_T("."));
        if (pos == 0)
            return String::Empty();
        else if (pos != String::InvalidIndex)
            return path.Substr(0, pos);
        else
            return path;
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

    String Path::GetShortName(const String& path, bool withExtension)
    {
        size_t pos = path.FindLast(_T("/\\"));
        String ret = String::Empty();
        if (pos != String::InvalidIndex && pos < path.Length() - 1)
        {
            ret = path.Substr(pos + 1, path.Length() - pos - 1);
            if (!withExtension)
                return RemoveExtension(ret);
        }

        //TODO: XXX/YYY/ need return YYY?
        return ret;
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

    DynamicArray<String> Path::SplitPath(const String& path)
    {
        return path.Split(_T("/\\"));
    }

    DynamicArray<UString> Path::SplitPath(const UString& path)
    {
        return path.SplitAny("/\\");
    }
}
