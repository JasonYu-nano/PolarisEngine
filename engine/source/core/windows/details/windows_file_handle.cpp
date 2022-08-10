#include "windows/windows_file_handle.hpp"
#include "log/logger.hpp"
#include "math/generic_math.hpp"
#include "math/limit.hpp"

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
    {
        WIN32_FIND_DATAW data;
        Handle = ::FindFirstFileExW(path.ToWChar(), FindExInfoStandard, &data, FindExSearchNameMatch, nullptr, 0);
    }

    bool WindowsFindFileHandle::FindNext() const
    {
        WIN32_FIND_DATAW data;
        return ::FindNextFileW(Handle, &data);
    }
}