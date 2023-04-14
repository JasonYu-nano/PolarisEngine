#include <corecrt_io.h>
#include "windows/windows_platform_file.hpp"
#include "foundation/regex.hpp"
#include "file_system/path.hpp"
#include "foundation/queue.hpp"
#include "windows/windows_file_handle.hpp"
#include "log/logger.hpp"
#include "file_system/file_system_log.hpp"

using namespace Engine;

namespace Engine
{
    static uint64 GetTimeStamp(const ::FILETIME& fileTime)
    {
        const uint64 k_UnixTimeStart = 0x019DB1DED53E8000;
        const uint64 k_TicksPerSecond = 10000000;

        ULARGE_INTEGER li;
        li.LowPart = fileTime.dwLowDateTime;
        li.HighPart = fileTime.dwHighDateTime;

        return (li.QuadPart - k_UnixTimeStart) / k_TicksPerSecond;
    }

    bool WindowsPlatformFile::MakeDir(const String& path)
    {
        return ::CreateDirectoryA(path.Data(), reinterpret_cast<SECURITY_ATTRIBUTES*>(NULL)) ||
               GetLastError() == ERROR_ALREADY_EXISTS;
    }

    bool WindowsPlatformFile::RemoveDir(const String& path)
    {
        return ::RemoveDirectoryA(path.Data());
    }

    bool WindowsPlatformFile::MakeFile(const String& path)
    {
        auto handle = ::CreateFileA(path.Data(), GENERIC_READ | GENERIC_WRITE,
                                    NULL, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL,NULL);
        bool ret = handle != INVALID_HANDLE_VALUE || GetLastError() == ERROR_FILE_EXISTS;
        CloseHandle(handle);
        return ret;
    }

    bool WindowsPlatformFile::RemoveFile(const String& path)
    {
        return ::DeleteFileA(path.Data());
    }

    bool WindowsPlatformFile::MoveFile(const String& from, const String& to)
    {
        return ::MoveFileA(from.Data(), to.Data());
    }

    bool WindowsPlatformFile::CopyFile(const String& from, const String& to)
    {
        return ::CopyFileA(from.Data(), to.Data(), true);
    }

    bool WindowsPlatformFile::FileExists(const String& path)
    {
        uint32 result = ::GetFileAttributesA(path.Data());
        return result != 0xFFFFFFFF && !(result & FILE_ATTRIBUTE_DIRECTORY);
    }

    bool WindowsPlatformFile::IsReadOnly(const String& filePath)
    {
        uint32 result = ::GetFileAttributesA(filePath.Data());
        return result != 0xFFFFFFFF && !(result & FILE_ATTRIBUTE_READONLY);
    }

    int64 WindowsPlatformFile::FileSize(const String& filePath)
    {
        WIN32_FILE_ATTRIBUTE_DATA info;
        if (!!GetFileAttributesExA(filePath.Data(), GetFileExInfoStandard, &info))
        {
            if ((info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
            {
                LARGE_INTEGER li;
                li.HighPart = info.nFileSizeHigh;
                li.LowPart = info.nFileSizeLow;
                return li.QuadPart;
            }
        }
        return -1;
    }

    bool WindowsPlatformFile::DirExists(const String& path)
    {
        uint32 result = ::GetFileAttributesA(path.Data());
        return result != 0xFFFFFFFF && (result & FILE_ATTRIBUTE_DIRECTORY);
    }

    FileTime WindowsPlatformFile::GetFileTime(const String& path)
    {
        auto handle = ::CreateFileA(path.Data(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
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

            ::CloseHandle(handle);
        }

        return ret;
    }

    Array<String>
    WindowsPlatformFile::QueryFiles(const String& searchPath, const String& regexExpr, bool recursion)
    {
        Array<String> ret;

        std::regex pattern(regexExpr.Data());

        Queue<String> searchQueue;
        searchQueue.emplace(searchPath);

        WIN32_FIND_DATAA data;

        while (!searchQueue.empty())
        {
            String& path = searchQueue.front();
            String queryPath = Path::Combine(path, "*");


            HANDLE handle = ::FindFirstFileA(queryPath.Data(), &data);
            if (handle != INVALID_HANDLE_VALUE)
            {
                do
                {
                    if (recursion && (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                    {
                        if (CharTraits<char>::Compare(data.cFileName, ".") != 0 &&
                            CharTraits<char>::Compare(data.cFileName, "..") != 0)
                        {
                            if (std::regex_match(data.cFileName, pattern))
                            {
                                ret.Add(Path::Combine(path, data.cFileName));
                            }
                            searchQueue.emplace(Path::Combine(path, data.cFileName));
                        }
                    }
                    else if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
                             std::regex_match(data.cFileName, pattern))
                    {
                        ret.Add(Path::Combine(path, data.cFileName));
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

    UniquePtr<IFileHandle> WindowsPlatformFile::OpenFile(const String& filePath, EFileAccess access, EFileShareMode mode)
    {
        int64 desiredAccess = 0;
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
        HANDLE handle = ::CreateFileA(filePath.Data(), desiredAccess, shareMode, nullptr, OPEN_EXISTING,
                                      FILE_ATTRIBUTE_NORMAL, nullptr);

        if (handle == INVALID_HANDLE_VALUE)
        {
            LOG_ERROR(FileSystem, "Open file: {} failed", filePath);
        }

        return MakeUnique<WindowsFileHandle>(handle);
    }
}