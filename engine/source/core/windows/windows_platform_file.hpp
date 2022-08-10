#pragma once

#include "file_system/platform_file_interface.hpp"

namespace Engine
{
    class CORE_API WindowsPlatformFile final : public IPlatformFile
    {
    public:
        WindowsPlatformFile() = default;
        virtual ~WindowsPlatformFile() = default;

        bool MakeDir(const UString& path) final;

        bool RemoveDir(const UString& path) final;

        bool MakeFile(const UString& path) final;

        bool RemoveFile(const UString& path) final;

        bool MoveFile(const UString& from, const UString& to) final;

        bool FileExists(const UString& path) final;

        bool IsReadOnly(const UString& filePath) final;

        int64 FileSize(const UString& filePath) final;

        bool DirExists(const UString& path) final;

        FileTime GetFileTime(const UString& path) final;

        DynamicArray<UString> QueryFiles(const UString& searchPath, const UString& regexExpr, bool recursion) final;

        UniquePtr<IFileHandle> OpenFile(const UString& fileName, EFileAccess access, EFileShareMode mode) final;

    private:
        uint32 GetLastError();
    };
}