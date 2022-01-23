#pragma once
#include "core_module.hpp"
#include "predefine/platform.hpp"
#include "foundation/string.hpp"
#include "file_system/file_time.hpp"

namespace Engine
{
    class CORE_API IPlatformFile
    {
    public:
        virtual ~IPlatformFile() = default;

        virtual bool MakeDir(const char_t* path) = 0;

        virtual bool RemoveDir(const char_t* path) = 0;

        virtual bool MakeFile(const char_t* path) = 0;

        virtual bool RemoveFile(const char_t* path) = 0;

        virtual bool FileExists(const char_t* path) = 0;

        virtual bool DirExists(const char_t* path) = 0;

        virtual FileTime GetFileTime(const char_t* path) = 0;

        virtual DynamicArray<String> QueryFiles(const char_t* searchPath, const char_t* regexExpr, bool recursion) = 0;
    };
}