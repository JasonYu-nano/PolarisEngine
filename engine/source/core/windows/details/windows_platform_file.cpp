#include <windows.h>
#include <corecrt_io.h>
#include "windows/windows_platform_file.hpp"
#include "foundation/regex.hpp"
#include "file_system/path.hpp"
#include "foundation/queue.hpp"
#include "foundation/char_utils.hpp"
#include "windows/windows_file_handle.hpp"

using namespace Engine;

namespace Engine
{
    static uint64 GetTimeStamp(const ::FILETIME& fileTime)
    {
        const int64 k_UnixTimeStart = 0x019DB1DED53E8000;
        const int64 k_TicksPerSecond = 10000000;

        LARGE_INTEGER li;
        li.LowPart = fileTime.dwLowDateTime;
        li.HighPart = fileTime.dwHighDateTime;

        return (li.QuadPart - k_UnixTimeStart) / k_TicksPerSecond;
    }

    bool WindowsPlatformFile::MakeDir(const char_t* path)
    {
        return CreateDirectory(path, reinterpret_cast<SECURITY_ATTRIBUTES*>(NULL)) ||
               GetLastError() == ERROR_ALREADY_EXISTS;
    }

    bool WindowsPlatformFile::MakeDir(const char* path)
    {
        return ::CreateDirectoryA(path, reinterpret_cast<SECURITY_ATTRIBUTES*>(NULL)) ||
               GetLastError() == ERROR_ALREADY_EXISTS;
    }

    bool WindowsPlatformFile::RemoveDir(const char_t* path)
    {
        return ::RemoveDirectory(path);
    }

    bool WindowsPlatformFile::RemoveDir(const char* path)
    {
        return ::RemoveDirectoryA(path);
    }

    bool WindowsPlatformFile::MakeFile(const char_t* path)
    {
        auto handle = ::CreateFile(path, GENERIC_READ | GENERIC_WRITE, NULL, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL,
                                   NULL);
        bool ret = handle != INVALID_HANDLE_VALUE || GetLastError() == ERROR_FILE_EXISTS;
        CloseHandle(handle);
        return ret;
    }

    bool WindowsPlatformFile::MakeFile(const char* path)
    {
        auto handle = ::CreateFileA(path, GENERIC_READ | GENERIC_WRITE, NULL, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL,
                                    NULL);
        bool ret = handle != INVALID_HANDLE_VALUE || GetLastError() == ERROR_FILE_EXISTS;
        CloseHandle(handle);
        return ret;
    }

    bool WindowsPlatformFile::RemoveFile(const char_t* path)
    {
        return ::DeleteFile(path);
    }

    bool WindowsPlatformFile::RemoveFile(const char* path)
    {
        return ::DeleteFileA(path);
    }

    bool WindowsPlatformFile::FileExists(const char_t* path)
    {
        uint32 result = ::GetFileAttributes(path);
        return result != 0xFFFFFFFF && !(result & FILE_ATTRIBUTE_DIRECTORY);
    }

    bool WindowsPlatformFile::FileExists(const char* path)
    {
        uint32 result = ::GetFileAttributesA(path);
        return result != 0xFFFFFFFF && !(result & FILE_ATTRIBUTE_DIRECTORY);
    }

    bool WindowsPlatformFile::DirExists(const char_t* path)
    {
        uint32 result = ::GetFileAttributes(path);
        return result != 0xFFFFFFFF && (result & FILE_ATTRIBUTE_DIRECTORY);
    }

    bool WindowsPlatformFile::DirExists(const char* path)
    {
        uint32 result = ::GetFileAttributesA(path);
        return result != 0xFFFFFFFF && (result & FILE_ATTRIBUTE_DIRECTORY);
    }

    FileTime WindowsPlatformFile::GetFileTime(const char_t* path)
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

    FileTime WindowsPlatformFile::GetFileTime(const char* path)
    {
        auto handle = ::CreateFileA(path, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
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

    DynamicArray<UString>
    WindowsPlatformFile::QueryFiles(const UString& searchPath, const UString& regexExpr, bool recursion)
    {
        DynamicArray<UString> ret;

        DynamicArray<char> regex = regexExpr.ToUtf8();
        std::regex pattern(regex.Data());

        Queue<UString> searchQueue;
        searchQueue.emplace(searchPath);

        WIN32_FIND_DATAA data;

        while (!searchQueue.empty())
        {
            UString& path = searchQueue.front();
            UString queryPath = Path::MakePath(path, "*");


            HANDLE handle = ::FindFirstFileA(queryPath.ToUtf8().Data(), &data);
            if (handle != INVALID_HANDLE_VALUE)
            {
                do
                {
                    if (recursion && (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                    {
                        if (CharUtils::Compare(data.cFileName, _T(".")) != 0 &&
                            CharUtils::Compare(data.cFileName, _T("..")) != 0)
                        {
                            if (std::regex_match(data.cFileName, pattern))
                            {
                                ret.Add(Path::MakePath(path, data.cFileName));
                            }
                            searchQueue.emplace(Path::MakePath(path, data.cFileName));
                        }
                    }
                    else if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
                             std::regex_match(data.cFileName, pattern))
                    {
                        ret.Add(Path::MakePath(path, data.cFileName));
                    }
                } while (::FindNextFileA(handle, &data));
            }
            ::FindClose(handle);
            searchQueue.pop();
        }

        return ret;
    }

    uint32 WindowsPlatformFile::GetLastError()
    {
        return (uint32) ::GetLastError();
    }

    UniquePtr<IFileHandle>
    WindowsPlatformFile::OpenFile(const char_t* fileName, EFileAccess access, EFileShareMode mode)
    {
        int32 desiredAccess = 0;
        int32 shareMode = 0;
        switch (access)
        {
            case EFileAccess::Read:
            {
                desiredAccess = GENERIC_READ;
                break;
            }
            case EFileAccess::Write:
            {
                desiredAccess = GENERIC_WRITE;
                break;
            }
            case EFileAccess::ReadWrite:
            {
                desiredAccess = GENERIC_READ | GENERIC_WRITE;
                break;
            }
            default:
            {
                //
            }
        }

        switch (mode)
        {
            case EFileShareMode::Read:
            {
                shareMode = FILE_SHARE_READ;
                break;
            }
            case EFileShareMode::Write:
            {
                shareMode = FILE_SHARE_WRITE;
                break;
            }
            case EFileShareMode::Delete:
            {
                shareMode = FILE_SHARE_DELETE;
                break;
            }
            default:
            {
                shareMode = 0;
            }
        }
        HANDLE handle = ::CreateFileW(fileName, desiredAccess, shareMode, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
                                      nullptr);
        return MakeUniquePtr<WindowsFileHandle>(handle);
    }
}