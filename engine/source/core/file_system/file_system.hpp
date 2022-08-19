#pragma once

#include "global.hpp"
#include "foundation/ustring.hpp"
#include "foundation/dynamic_array.hpp"
#include "foundation/smart_ptr.hpp"
#include "file_system/platform_file_interface.hpp"
#include <filesystem>

namespace Engine
{
    class CORE_API FileSystem
    {
    public:
        static bool MakeDir(const UString& path);

        /**
         * @brief remove an empty directory
         */
        static bool RemoveDir(const UString& path);

        static bool DirExists(const UString& path);

        static inline bool IsDirectory(const UString& path)
        {
            return DirExists(path);
        }

        static bool MakeFile(const UString& path);

        static bool RemoveFile(const UString& path);

        static bool MoveFile(const UString& from, const UString& to);

        static bool CopyFile(const UString& from, const UString& to);

        static bool FileExists(const UString& path);

        static inline bool IsFile(const UString& path)
        {
            return FileExists(path);
        }

        static bool MakeDirTree(const UString& path);

        static bool ClearDir(const UString& path);

        static FileTime GetFileTime(const UString& path);

        /**
         * @brief query file (and directory) by regex
         * @param searchPath path for start search
         * @param regex
         * @param recursion recursion search or not, default false
         * @return files or directories match regex
         */
        static DynamicArray<UString> QueryFiles(const UString& searchPath, const UString& regex, bool recursion = false);

        static UString GetEngineRootPath();

        static void ReadFileToBinary(const UString& fileName, DynamicArray64<uint8>& outBinary);

        class DirectoryIterImpl
        {
        public:
            explicit DirectoryIterImpl(const UString& path, bool recursive);

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

            DirectoryIterator(UString path, bool recursive = false);

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

    //class CORE_API Recursive
}