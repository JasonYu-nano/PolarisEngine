#pragma once

#include "global.hpp"
#include "file_system/file_time.hpp"
#include "foundation/smart_ptr.hpp"
#include "file_system/file_handle_interface.hpp"

namespace Engine
{
    class CORE_API IPlatformFile
    {
    public:
        virtual ~IPlatformFile() = default;

        virtual bool MakeDir(const String& path) = 0;

        virtual bool RemoveDir(const String& path) = 0;

        virtual bool DirExists(const String& path) = 0;

        virtual bool MakeFile(const String& path) = 0;

        virtual bool RemoveFile(const String& path) = 0;

        virtual bool MoveFile(const String& from, const String& to) = 0;

        virtual bool CopyFile(const String& from, const String& to) = 0;

        virtual bool FileExists(const String& path) = 0;

        virtual bool IsReadOnly(const String& filePath) = 0;

        virtual int64 FileSize(const String& filePath) = 0;

        virtual FileTime GetFileTime(const String& path) = 0;

        virtual Array<String> QueryFiles(const String& searchPath, const String& regexExpr, bool recursion) = 0;

        virtual UniquePtr<IFileHandle> OpenFile(const String& fileName, EFileAccess access, EFileShareMode mode) = 0;
    };
}