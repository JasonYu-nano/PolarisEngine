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
        static FileSystem& Get();

        bool MakeDir(const UString& path);

        /**
         * @brief remove an empty directory
         */
        bool RemoveDir(const UString& path);

        bool DirExists(const UString& path);

        inline bool IsDirectory(const UString& path)
        {
            return DirExists(path);
        }

        bool MakeFile(const UString& path);

        bool RemoveFile(const UString& path);

        bool MoveFile(const UString& from, const UString& to);

        bool CopyFile(const UString& from, const UString& to);

        bool FileExists(const UString& path);

        inline bool IsFile(const UString& path)
        {
            return FileExists(path);
        }

        bool MakeDirTree(const UString& path);

        bool ClearDir(const UString& path);

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

        void ReadFileToBinary(const UString& fileName, DynamicArray64<uint8>& outBinary);

        class DirectoryIterImpl
        {
        public:
            explicit DirectoryIterImpl(const UString& path);

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

            DirectoryIterator(const UString& path);

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
        FileSystem();

        UniquePtr<IPlatformFile> PlatformFile;
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