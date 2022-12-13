#pragma once

#include "windows/minimal_windows.hpp"
#include "file_system/file_handle_interface.hpp"

namespace Engine
{
    class CORE_API WindowsFileHandle final : public IFileHandle
    {
    public:
        explicit WindowsFileHandle(HANDLE handle) : Handle(handle)
        {
            CalcFileSize();
        }

        ~WindowsFileHandle() final;

        int64 GetSize() const final;

        bool Read(uint8* dest, int64 size) final;
    private:
        void CalcFileSize();

        void UpdateOverlapped();

    private:
        HANDLE Handle{ nullptr };
        int64 Size{ 0 };
        uint64 PosInFile{ 0 };
        OVERLAPPED Overlapped{ 0 };
    };

    class CORE_API WindowsFindFileHandle final : public IFindFileHandle
    {
    public:
        WindowsFindFileHandle(const String& path);

        ~WindowsFindFileHandle() final;

        bool FindNext(DirectoryEntry& entry) final;

    private:
        HANDLE Handle{ nullptr };
        String NormalizedPath;
    };

    class CORE_API WindowsRecursiveFindFileHandle final : public IFindFileHandle
    {
    public:
        WindowsRecursiveFindFileHandle(const String& path);

        ~WindowsRecursiveFindFileHandle() final;

        bool FindNext(DirectoryEntry& entry) final;

    private:
        HANDLE FindTop(WIN32_FIND_DATAA& data);

        HANDLE Handle{ nullptr };
        String NormalizedPath;
        DynamicArray<String> RecursionDirectories;
    };

    typedef WindowsFileHandle PlatformFileHandle;
    typedef WindowsFindFileHandle PlatformFindFileHandle;
    typedef WindowsRecursiveFindFileHandle PlatformRecursiveFindFileHandle;
}