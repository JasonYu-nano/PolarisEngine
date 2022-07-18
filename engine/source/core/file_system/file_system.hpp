#pragma once
#include "global.hpp"
#include "foundation/string.hpp"
#include "foundation/ustring.hpp"
#include "foundation/dynamic_array.hpp"
#include "foundation/smart_ptr.hpp"
#include "file_system/platform_file_interface.hpp"

namespace Engine
{
    class CORE_API FileSystem
    {
    public:
        static FileSystem& Get();

        bool MakeDir(const String& path);

        bool MakeDir(const char* path);
        inline bool MakeDir(const UString& path);

        /**
         * @brief remove an empty directory
         */
        inline bool RemoveDir(const String& path);

        bool RemoveDir(const char* path);
        inline bool RemoveDir(const UString& path);

        bool MakeFile(const char* path);
        inline bool MakeFile(const UString& path);

        inline bool RemoveFile(const String& path);

        bool RemoveFile(const char* path);
        inline bool RemoveFile(const UString& path);

        inline bool DirExists(const String& path);

        bool DirExists(const char* path);
        inline bool DirExists(const UString& path);

        inline bool FileExists(const String& path);

        bool FileExists(const char* path);
        inline bool FileExists(const UString& path);

        inline bool IsDirectory(const String& path);

        inline bool IsFile(const String& path);

        bool MakeDirTree(const String& path);
        bool MakeDirTree(const char* path);

        bool ClearDir(const String& path);

        FileTime GetFileTime(const char* path);
        inline FileTime GetFileTime(const UString& path);

        /**
         * @brief query file (and directory) by regex
         * @param searchPath path for start search
         * @param regex
         * @param recursion recursion search or not, default false
         * @return files or directories match regex
         */
        DynamicArray<String> QueryFiles(const String& searchPath, const String& regex, bool recursion = false);

        String GetEngineRootPath() const;

        void ReadFileToBinary(const String& fileName, DynamicArray64<uint8>& outBinary);

    private:
        FileSystem();

        UniquePtr<IPlatformFile> PlatformFile;
    };

    bool FileSystem::MakeDir(const UString& path)
    {
        return MakeDir(path.ToUtf8().Data());
    }

    bool FileSystem::RemoveDir(const UString& path)
    {
        return RemoveDir(path.ToUtf8().Data());
    }

    bool FileSystem::MakeFile(const UString& path)
    {
        return MakeFile(path.ToUtf8().Data());
    }

    bool FileSystem::RemoveFile(const UString& path)
    {
        return RemoveFile(path.ToUtf8().Data());
    }

    bool FileSystem::DirExists(const UString& path)
    {
        return DirExists(path.ToUtf8().Data());
    }

    bool FileSystem::FileExists(const UString& path)
    {
        return FileExists(path.ToUtf8().Data());
    }

    FileTime FileSystem::GetFileTime(const UString& path)
    {
        return GetFileTime(path.ToUtf8().Data());
    }
}