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
        bool RemoveDir(const UString& path);

        bool MakeFile(const char* path);
        bool MakeFile(const UString& path);

        inline bool RemoveFile(const String& path);

        bool RemoveFile(const char* path);
        bool RemoveFile(const UString& path);

        inline bool DirExists(const String& path);

        bool DirExists(const char* path);
        bool DirExists(const UString& path);

        inline bool FileExists(const String& path);

        bool FileExists(const char* path);
        bool FileExists(const UString& path);

        inline bool IsDirectory(const String& path);
        inline bool IsDirectory(const UString& path)
        {
            return DirExists(path);
        }

        inline bool IsFile(const String& path);
        inline bool IsFile(const UString& path)
        {
            return FileExists(path);
        }

        bool MakeDirTree(const String& path);
        bool MakeDirTree(const UString& path);

        bool ClearDir(const UString& path);

        FileTime GetFileTime(const char* path);
        FileTime GetFileTime(const UString& path);

        /**
         * @brief query file (and directory) by regex
         * @param searchPath path for start search
         * @param regex
         * @param recursion recursion search or not, default false
         * @return files or directories match regex
         */
        DynamicArray<UString> QueryFiles(const UString& searchPath, const UString& regex, bool recursion = false);

        UString GetEngineRootPath() const;

        void ReadFileToBinary(const String& fileName, DynamicArray64<uint8>& outBinary);

    private:
        FileSystem();

        UniquePtr<IPlatformFile> PlatformFile;
    };
}