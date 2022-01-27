#pragma once
#include "predefine/platform.hpp"
#include "foundation/string.hpp"
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

        /**
         * @brief remove an empty directory
         */
        inline bool RemoveDir(const String& path);

        inline bool MakeFile(const String& path);

        inline bool RemoveFile(const String& path);

        inline bool DirExists(const String& path);

        inline bool FileExists(const String& path);

        inline bool IsDirectory(const String& path);

        inline bool IsFile(const String& path);

        bool MakeDirTree(const String& path);

        bool ClearDir(const String& path);

        inline FileTime GetFileTime(const String& path);

        /**
         * @brief query file (and directory) by regex
         * @param searchPath path for start search
         * @param regex
         * @param recursion recursion search or not, default false
         * @return files or directories match regex
         */
        DynamicArray<String> QueryFiles(const String& searchPath, const String& regex, bool recursion = false);

        String GetEngineRootPath() const;

        void ReadFileToBinary(const String& fileName, DynamicArrayU64<uint8>& outBinary);

    private:
        FileSystem();

        UniquePtr<IPlatformFile> PlatformFile;
    };
}