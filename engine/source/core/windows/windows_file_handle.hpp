#pragma once

#include <windows.h>
#include "file_system/file_handle_interface.hpp"

namespace Engine
{
    class CORE_API WindowsFileHandle : public IFileHandle
    {
    public:
        explicit WindowsFileHandle(HANDLE handle) : Handle(handle)
        {
            CalcFileSize();
        }

        ~WindowsFileHandle() final;

        int64 GetSize() const final;

        bool Read(uint8* dest, uint64 size) final;
    private:
        void CalcFileSize();

        void UpdateOverlapped();

    private:
        HANDLE Handle{ nullptr };
        int64 Size{ 0 };
        uint64 PosInFile{ 0 };
        OVERLAPPED Overlapped;
    };

    typedef WindowsFileHandle PlatformFileHandle;
}