#include "windows/windows_file_handle.hpp"
#include "log/logger.hpp"
#include "math/generic_math.hpp"
#include "math/limit.hpp"
#include "file_system/path.hpp"
#include "windows/windows_utils.hpp"

namespace Engine
{
    WindowsFileHandle::~WindowsFileHandle()
    {
        bool result = CloseHandle(Handle);
        CLOG(!result, "FileSystem", Error, "Close windows file handle failed");
    }

    int64 WindowsFileHandle::GetSize() const
    {
        return Size;
    }

    void WindowsFileHandle::CalcFileSize()
    {
        LARGE_INTEGER size;
        GetFileSizeEx(Handle, &size);
        Size = size.QuadPart;
    }

    bool WindowsFileHandle::Read(uint8* dest, int64 size)
    {
        if (size <= 0)
        {
            return false;
        }

        uint64 totalSize = 0;
        do
        {
            DWORD readBytes = 0;
            DWORD bytesToRead = (DWORD)Math::Min(size, static_cast<int64>(MAX_DWORD));
            if (!::ReadFile(Handle, dest, bytesToRead, &readBytes, &Overlapped))
            {
                LOG_ERROR("FileSystem", "Read file meet error, error code: {0}", ::GetLastError());
            }
            size -= bytesToRead;
            dest += bytesToRead;
            totalSize += readBytes;
            PosInFile += readBytes;
            UpdateOverlapped();
        }
        while(size > 0);

        return totalSize == size;
    }

    void WindowsFileHandle::UpdateOverlapped()
    {
        ULARGE_INTEGER pos;
        pos.QuadPart = PosInFile;
        Overlapped.Offset = pos.LowPart;
        Overlapped.OffsetHigh = pos.HighPart;
    }

    WindowsFindFileHandle::WindowsFindFileHandle(const UString& path)
        : NormalizedPath(Path::Normalize(path))
    {}

    WindowsFindFileHandle::~WindowsFindFileHandle()
    {
        if (Handle && Handle != INVALID_HANDLE_VALUE)
        {
            ::FindClose(Handle);
            Handle = nullptr;
        }
    }

    bool WindowsFindFileHandle::FindNext(DirectoryEntry& entry)
    {
        WIN32_FIND_DATAW data;
        bool ret = false;
        do
        {
            if (Handle == nullptr)
            {
                UString findPath = Path::Normalize(NormalizedPath) / "*";
                Handle = ::FindFirstFileW(findPath.ToWChar(), &data);
                ret = Handle != INVALID_HANDLE_VALUE;
            }
            else if (Handle != INVALID_HANDLE_VALUE)
            {
                ret = ::FindNextFileW(Handle, &data);
            }
        }
        while (ret && (!CharTraits<wchar_t>::Compare(data.cFileName, L".") ||
                       !CharTraits<wchar_t>::Compare(data.cFileName, L"..")));

        if (ret)
        {
            UString entryPath = NormalizedPath / UString(reinterpret_cast<const UChar*>(data.cFileName));

            ULARGE_INTEGER fileSize;
            fileSize.HighPart = data.nFileSizeHigh;
            fileSize.LowPart = data.nFileSizeLow;

            FileStat status{FileTimeToTimestamp(data.ftLastWriteTime),
                            FileTimeToTimestamp(data.ftLastAccessTime),
                            FileTimeToTimestamp(data.ftCreationTime),
                            static_cast<int64>(fileSize.QuadPart),
                            (data.dwFileAttributes & FILE_ATTRIBUTE_READONLY) != 0,
                            (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0,
                            true
            };

            entry = DirectoryEntry(status, entryPath);
        }

        return ret;
    }

    WindowsRecursiveFindFileHandle::WindowsRecursiveFindFileHandle(const UString& path)
    {
        RecursionDirectories.Push(Path::Normalize(path));
    }

    WindowsRecursiveFindFileHandle::~WindowsRecursiveFindFileHandle()
    {
        if (Handle && Handle != INVALID_HANDLE_VALUE)
        {
            ::FindClose(Handle);
            Handle = nullptr;
        }
    }

    bool WindowsRecursiveFindFileHandle::FindNext(DirectoryEntry& entry)
    {
        WIN32_FIND_DATAW data;
        bool ret = true;
        do
        {
            if (Handle == nullptr)
            {
                Handle = FindTop(data);
                ret = Handle != INVALID_HANDLE_VALUE;
            }
            else if (Handle != INVALID_HANDLE_VALUE)
            {
                ret = ::FindNextFileW(Handle, &data);
                if (!ret && !RecursionDirectories.Empty())
                {
                    ::FindClose(Handle);
                    Handle = FindTop(data);
                    ret = Handle != INVALID_HANDLE_VALUE;
                }
            }
        }
        while (ret && (!CharTraits<wchar_t>::Compare(data.cFileName, L".") ||
                       !CharTraits<wchar_t>::Compare(data.cFileName, L"..")));

        if (ret)
        {
            UString entryPath = NormalizedPath / UString(reinterpret_cast<const UChar*>(data.cFileName));
            if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                RecursionDirectories.Push(entryPath);
            }

            ULARGE_INTEGER fileSize;
            fileSize.HighPart = data.nFileSizeHigh;
            fileSize.LowPart = data.nFileSizeLow;

            FileStat status{FileTimeToTimestamp(data.ftLastWriteTime),
                            FileTimeToTimestamp(data.ftLastAccessTime),
                            FileTimeToTimestamp(data.ftCreationTime),
                            static_cast<int64>(fileSize.QuadPart),
                            (data.dwFileAttributes & FILE_ATTRIBUTE_READONLY) != 0,
                            (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0,
                            true
            };

            entry = DirectoryEntry(status, entryPath);
        }

        return ret;
    }

    HANDLE WindowsRecursiveFindFileHandle::FindTop(WIN32_FIND_DATAW& data)
    {
        NormalizedPath = RecursionDirectories.Pop();
        UString path = NormalizedPath / "*";
        Handle = ::FindFirstFileW(path.ToWChar(), &data);
        return Handle;
    }
}