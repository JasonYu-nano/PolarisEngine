#pragma once
#include "core_module.hpp"
#include "predefine/platform.hpp"
#include "foundation/string.hpp"
#include "foundation/array.hpp"
#include "file_system/file_time.hpp"

namespace Engine
{
    class CORE_API IPlatformFile
    {
    public:
        virtual ~IPlatformFile() = default;

        virtual bool MakeDir(const tchar* path) = 0;

        virtual bool RemoveDir(const tchar* path) = 0;

        virtual bool MakeFile(const tchar* path) = 0;

        virtual bool RemoveFile(const tchar* path) = 0;

        virtual bool FileExists(const tchar* path) = 0;

        virtual bool DirExists(const tchar* path) = 0;

        virtual FileTime GetFileTime(const tchar* path) = 0;

        virtual Vector<String> QueryFiles(const tchar* searchPath, const tchar* regexExpr, bool recursion = false) = 0;
    };
}