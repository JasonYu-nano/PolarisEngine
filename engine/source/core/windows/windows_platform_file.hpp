#pragma once

#include "file_system/platform_file_interface.hpp"

namespace Engine
{
    class CORE_API WindowsPlatformFile : public IPlatformFile
    {
    public:
        WindowsPlatformFile() = default;
        virtual ~WindowsPlatformFile() = default;

        virtual bool MakeDir(const char* path) final;
        bool MakeDir(const char_t* path) override;

        virtual bool RemoveDir(const char* path) final;
        bool RemoveDir(const char_t* path) override;

        virtual bool MakeFile(const char* path) final;
        bool MakeFile(const char_t* path) override;

        virtual bool RemoveFile(const char* path) final;
        bool RemoveFile(const char_t* path) override;

        virtual bool FileExists(const char* path) final;
        bool FileExists(const char_t* path) override;

        virtual bool DirExists(const char* path) final;
        bool DirExists(const char_t* path) override;

        virtual FileTime GetFileTime(const char* path) final;
        FileTime GetFileTime(const char_t* path) override;

        DynamicArray<UString> QueryFiles(const UString& searchPath, const UString& regexExpr, bool recursion) override;

        UniquePtr<IFileHandle> OpenFile(const char_t* fileName, EFileAccess access, EFileShareMode mode);

    private:
        uint32 GetLastError();
    };
}