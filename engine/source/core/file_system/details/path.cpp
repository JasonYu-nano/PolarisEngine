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

    String Path::GetExtension(const String& path)
    {
        size_t pos = path.FindLast(_T("."));
        if (pos != String::InvalidIndex && pos < path.Length() - 1)
        {
            return path.Substr(pos, path.Length() - pos);
        }
        return String::Empty();
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

    DynamicArray<String> Path::SplitPath(const String& path)
    {
        return path.Split(_T("/\\"));
    }
}
