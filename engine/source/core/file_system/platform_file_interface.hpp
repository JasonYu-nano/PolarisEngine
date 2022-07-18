#pragma once

#include "global.hpp"
#include "foundation/string.hpp"
#include "file_system/file_time.hpp"
#include "file_system/file_handle_interface.hpp"

namespace Engine
{
    class CORE_API IPlatformFile
    {
    public:
        virtual ~IPlatformFile() = default;

        virtual bool MakeDir(const char* path) = 0;
        virtual bool MakeDir(const char_t* path) = 0;

        virtual bool RemoveDir(const char* path) = 0;
        virtual bool RemoveDir(const char_t* path) = 0;

        virtual bool MakeFile(const char* path) = 0;
        virtual bool MakeFile(const char_t* path) = 0;

        virtual bool RemoveFile(const char* path) = 0;
        virtual bool RemoveFile(const char_t* path) = 0;

        virtual bool FileExists(const char* path) = 0;
        virtual bool FileExists(const char_t* path) = 0;

        virtual bool DirExists(const char* path) = 0;
        virtual bool DirExists(const char_t* path) = 0;

        virtual FileTime GetFileTime(const char* path) = 0;
        virtual FileTime GetFileTime(const char_t* path) = 0;

        virtual DynamicArray<String> QueryFiles(const char_t* searchPath, const char_t* regexExpr, bool recursion) = 0;

        virtual UniquePtr<IFileHandle> OpenFile(const char_t* fileName, EFileAccess access, EFileShareMode mode) = 0;
    };
}