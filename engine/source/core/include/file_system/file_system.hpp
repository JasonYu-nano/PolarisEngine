#pragma once

#include "global.hpp"
#include "foundation/array.hpp"
#include "foundation/smart_ptr.hpp"
#include "file_system/platform_file_interface.hpp"
#include "foundation/string.hpp"

namespace Engine
{
    class CORE_API FileSystem
    {
    public:
        static bool MakeDir(const String& path);

        /**
         * @brief remove an empty directory
         */
        static bool RemoveDir(const String& path);

        static bool DirExists(const String& path);

        static inline bool IsDirectory(const String& path)
        {
            return DirExists(path);
        }

        static bool MakeFile(const String& path);

        static bool RemoveFile(const String& path);

        static bool MoveFile(const String& from, const String& to);

        static bool CopyFile(const String& from, const String& to);

        static bool FileExists(const String& path);

        static inline bool IsFile(const String& path)
        {
            return FileExists(path);
        }

        static bool MakeDirTree(const String& path);

        static bool ClearDir(const String& path);

        static FileTime GetFileTime(const String& path);

        /**
         * @brief query file (and directory) by regex
         * @param searchPath path for start search
         * @param regex
         * @param recursion recursion search or not, default false
         * @return files or directories match regex
         */
        static Array<String> QueryFiles(const String& searchPath, const String& regex, bool recursion = false);

        static String GetEngineRootPath();

        static String GetEngineSaveDir();

        static String GetEngineIntermediateDir();

        static void ReadFileToBinary(const String& fileName, Array64<uint8>& outBinary);

        static String ReadFileToString(const String& fileName);

        static void SetupExceptionHandle();

        class DirectoryIterImpl
        {
        public:
            explicit DirectoryIterImpl(const String& path, bool recursive);

            bool FindNext();

        public:
            DirectoryEntry Entry;

        private:
            UniquePtr<IFindFileHandle> FileHandle;
        };

        class CORE_API DirectoryIterator
        {
        public:
            DirectoryIterator() noexcept = default;

            DirectoryIterator(const String& path, bool recursive = false);

            NODISCARD DirectoryEntry& operator* () const
            {
                return Impl->Entry;
            }

            NODISCARD const DirectoryEntry* operator-> () const
            {
                return &Impl->Entry;
            }

            DirectoryIterator& operator++()
            {
                if (!Impl->FindNext())
                {
                    Impl.reset();
                }
                return *this;
            }

            NODISCARD bool operator== (const DirectoryIterator& other) const
            {
                return this->Impl == other.Impl;
            }

            NODISCARD bool operator!= (const DirectoryIterator& other) const
            {
                return this->Impl != other.Impl;
            }

        private:
            SharedPtr<DirectoryIterImpl> Impl;
        };

    private:
        static UniquePtr<IPlatformFile> PlatformFile;
    };

    NODISCARD inline FileSystem::DirectoryIterator begin(FileSystem::DirectoryIterator iter) noexcept
    {
        return iter;
    }

    NODISCARD inline FileSystem::DirectoryIterator end(FileSystem::DirectoryIterator iter) noexcept
    {
        return {};
    }
}