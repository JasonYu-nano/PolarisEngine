#include "windows/windows_file_handle.hpp"
#include "log/logger.hpp"
#include "math/generic_math.hpp"
#include "math/limit.hpp"
#include "windows/details/windows_utils.hpp"

namespace Engine
{
    WindowsFileHandle::~WindowsFileHandle()
    {
        bool result = CloseHandle(Handle);
        CLOG(result, "FileSystem", Error, "Close windows file handle failed");
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

    bool WindowsFileHandle::Read(uint8* dest, uint64 size)
    {
        uint64 totalSize = 0;
        do
        {
            DWORD readBytes = 0;
            DWORD bytesToRead = (DWORD)Math::Min(size, static_cast<uint64>(MAX_DWORD));
            if (!::ReadFile(Handle, dest, bytesToRead, &readBytes, &Overlapped))
            {
                LOG_ERROR("FileSystem", "Read file meet error");
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
        : FindPath(path)
    {}

    bool WindowsFindFileHandle::FindNext(DirectoryEntry& entry)
    {
        WIN32_FIND_DATAW data;
        if (Handle == nullptr)
        {
            Handle = ::FindFirstFileExW(FindPath.ToWChar(), FindExInfoStandard, &data, FindExSearchNameMatch, nullptr, 0);
        }
        else if (Handle != INVALID_HANDLE_VALUE)
        {
            ::FindNextFileW(Handle, &data);
        }

        if (Handle != INVALID_HANDLE_VALUE)
        {
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

            entry = DirectoryEntry(status, UString(reinterpret_cast<const UChar*>(data.cFileName)));
        }

        return Handle != INVALID_HANDLE_VALUE;
    }
}