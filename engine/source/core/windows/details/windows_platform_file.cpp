#include "precompiled_core.hpp"
#include <corecrt_io.h>
#include "foundation/regex.hpp"
#include "windows/windows_platform_file.hpp"
#include "file_system/path.hpp"
#include "foundation/queue.hpp"

using namespace Engine;

namespace Engine
{

    bool WindowsPlatformFile::MakeDir(const tchar* path)
    {
        return CreateDirectory(path, reinterpret_cast<SECURITY_ATTRIBUTES*>(NULL)) || GetLastError() == ERROR_ALREADY_EXISTS;
    }

    bool WindowsPlatformFile::RemoveDir(const tchar* path)
    {
        return ::RemoveDirectory(path);
    }

    bool WindowsPlatformFile::MakeFile(const tchar* path)
    {
        auto handle = ::CreateFile(path, GENERIC_READ | GENERIC_WRITE, NULL, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
        bool ret = handle != INVALID_HANDLE_VALUE || GetLastError() == ERROR_FILE_EXISTS;
        CloseHandle(handle);
        return ret;
    }

    bool WindowsPlatformFile::RemoveFile(const tchar* path)
    {
        return ::DeleteFile(path);
    }

    bool WindowsPlatformFile::FileExists(const tchar* path)
    {
        uint32 result = ::GetFileAttributes(path);
        return result != 0xFFFFFFFF && !(result & FILE_ATTRIBUTE_DIRECTORY);
    }

    bool WindowsPlatformFile::DirExists(const tchar* path)
    {
        uint32 result = ::GetFileAttributes(path);
        return result != 0xFFFFFFFF && (result & FILE_ATTRIBUTE_DIRECTORY);
    }

    FileTime WindowsPlatformFile::GetFileTime(const tchar* path)
    {
        auto handle = ::CreateFile(path, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        FileTime ret;
        if (handle != INVALID_HANDLE_VALUE)
        {
            FILETIME fCreationTime;
            FILETIME fLastAccessTime;
            FILETIME fLastWriteTime;
            if (::GetFileTime(handle, &fCreationTime, &fLastAccessTime, &fLastWriteTime))
            {
                ret.CreationTime = GetTimeStamp(fCreationTime);
                ret.LastAccessTime = GetTimeStamp(fLastAccessTime);
                ret.LastModifyTime = GetTimeStamp(fLastWriteTime);
            }
        }

        return ret;
    }

    Vector<String> WindowsPlatformFile::QueryFiles(const tchar* searchPath, const tchar* regexExpr, bool recursion)
    {
        Vector<String> ret;

        Regex pattern(regexExpr);

        Queue<String> searchQueue;
        searchQueue.emplace(searchPath);

        WIN32_FIND_DATA data;

        while (!searchQueue.empty())
        {
            String& path = searchQueue.front();

            HANDLE handle = ::FindFirstFile(*Path::MakePath(path, TC("*")), &data);
            if (handle != INVALID_HANDLE_VALUE)
            {
                do
                {
                    if (recursion && (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                    {
                        if (StrCmp(data.cFileName, TC(".")) != 0 && StrCmp(data.cFileName, TC("..")) != 0)
                        {
                            if (std::regex_match(data.cFileName, pattern))
                            {
                                ret.emplace_back(Path::MakePath(path, data.cFileName));
                            }
                            searchQueue.emplace(Path::MakePath(path, data.cFileName));
                        }
                    }
                    else if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && std::regex_match(data.cFileName, pattern))
                    {
                        ret.emplace_back(Path::MakePath(path, data.cFileName));
                    }
                } while (::FindNextFile(handle, &data));
            }
            ::FindClose(handle);
            searchQueue.pop();
        }

        return ret;
    }

    uint32 WindowsPlatformFile::GetLastError()
    {
        return (uint32)::GetLastError();
    }

    uint64 WindowsPlatformFile::GetTimeStamp(const ::FILETIME& fileTime)
    {
        const int64 k_UnixTimeStart = 0x019DB1DED53E8000;
        const int64 k_TicksPerSecond = 10000000;

        LARGE_INTEGER li;
        li.LowPart = fileTime.dwLowDateTime;
        li.HighPart = fileTime.dwHighDateTime;

        return (li.QuadPart - k_UnixTimeStart) / k_TicksPerSecond;
    }
}
