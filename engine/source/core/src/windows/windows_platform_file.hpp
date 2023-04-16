#pragma once

#include "file_system/platform_file_interface.hpp"

namespace Engine
{
    class CORE_API WindowsPlatformFile final : public IPlatformFile
    {
    public:
        WindowsPlatformFile() = default;
        virtual ~WindowsPlatformFile() = default;

        bool MakeDir(const String& path) final;

        bool RemoveDir(const String& path) final;

        bool MakeFile(const String& path) final;

        bool RemoveFile(const String& path) final;

        bool MoveFile(const String& from, const String& to) final;

        bool CopyFile(const String& from, const String& to) final;

        bool FileExists(const String& path) final;

        bool IsReadOnly(const String& filePath) final;

        int64 FileSize(const String& filePath) final;

        bool DirExists(const String& path) final;

        FileTime GetFileTime(const String& path) final;

        Array<String> QueryFiles(const String& searchPath, const String& regexExpr, bool recursion) final;

        UniquePtr<IFileHandle> OpenFile(const String& fileName, EFileAccess access, EFileShareMode mode) final;

        void SetupExceptionHandle() final;
    private:
        uint32 GetLastError();
    };
}