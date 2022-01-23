#pragma once
#include "file_system/platform_file_interface.hpp"
#include <windows.h>

namespace Engine
{
    class CORE_API WindowsPlatformFile : public IPlatformFile
    {
    public:
        WindowsPlatformFile() = default;
        virtual ~WindowsPlatformFile() = default;

        bool MakeDir(const char_t* path) override;

        bool RemoveDir(const char_t* path) override;

        bool MakeFile(const char_t* path) override;

        bool RemoveFile(const char_t* path) override;

        bool FileExists(const char_t* path) override;

        bool DirExists(const char_t* path) override;

        FileTime GetFileTime(const char_t* path) override;

        DynamicArray<String> QueryFiles(const char_t* searchPath, const char_t* regexExpr, bool recursion) override;
    private:
        uint32 GetLastError();

        uint64 GetTimeStamp(const ::FILETIME& fileTime);
    };
}