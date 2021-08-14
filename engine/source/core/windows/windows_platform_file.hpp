#pragma once
#include "core/file_system/platform_file_interface.hpp"

namespace Nano
{
    class ENGINE_API WindowsPlatformFile : public IPlatformFile
    {
    public:
        WindowsPlatformFile() = default;
        ~WindowsPlatformFile() = default;

        bool MakeDir(const tchar* path) override;

        bool RemoveDir(const tchar* path) override;

        bool MakeFile(const tchar* path) override;

        bool RemoveFile(const tchar* path) override;

        bool FileExists(const tchar* path) override;

        bool DirExists(const tchar* path) override;

        FileTime GetFileTime(const tchar* path) override;

        Vector<String> QueryFiles(const tchar* searchPath, const tchar* regexExpr, bool recursion = false) override;
    private:
        uint32 GetLastError();

        size_t GetTimeStamp(const ::FILETIME& fileTime);
    };
}