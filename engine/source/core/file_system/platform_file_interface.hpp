#pragma once

#include "global.hpp"
#include "foundation/ustring.hpp"
#include "file_system/file_time.hpp"
#include "foundation/smart_ptr.hpp"
#include "file_system/file_handle_interface.hpp"

namespace Engine
{
    class CORE_API IPlatformFile
    {
    public:
        virtual ~IPlatformFile() = default;

        virtual bool MakeDir(const UString& path) = 0;

        virtual bool RemoveDir(const UString& path) = 0;

        virtual bool DirExists(const UString& path) = 0;

        virtual bool MakeFile(const UString& path) = 0;

        virtual bool RemoveFile(const UString& path) = 0;

        virtual bool MoveFile(const UString& from, const UString& to) = 0;

        virtual bool FileExists(const UString& path) = 0;

        virtual bool IsReadOnly(const UString& filePath) = 0;

        virtual int64 FileSize(const UString& filePath) = 0;

        virtual FileTime GetFileTime(const UString& path) = 0;

        virtual DynamicArray<UString> QueryFiles(const UString& searchPath, const UString& regexExpr, bool recursion) = 0;

        virtual UniquePtr<IFileHandle> OpenFile(const UString& fileName, EFileAccess access, EFileShareMode mode) = 0;
    };
}