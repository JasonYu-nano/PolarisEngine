//#include "precompiled_core.hpp"
#include "file_system/file_system.hpp"
#include "file_system/path.hpp"

#if PLATFORM_WINDOWS
#include "windows/windows_platform_file.hpp"
#include "windows/windows_file_handle.hpp"
#endif

namespace Engine
{
    FileSystem& FileSystem::Get()
    {
        static FileSystem instance;
        return instance;
    }

    bool FileSystem::MakeDir(const UString& path)
    {
        return PlatformFile->MakeDir(path);
    }

    bool FileSystem::RemoveDir(const UString& path)
    {
        return PlatformFile->RemoveDir(path);
    }

    bool FileSystem::MakeDirTree(const UString& path)
    {
        DynamicArray<UString> dirs = Path::SplitPath(path);
        if (dirs.Size() <= 0)
        {
            return false;
        }

        UString destPath;
        for (int32 index = 0; index < dirs.Size(); index++)
        {
            destPath = Path::MakePath(destPath, dirs[index]);
            if (DirExists(destPath))
            {
                continue;
            }
            else if (MakeDir(destPath))
            {
                continue;
            }
            else
            {
                return false;
            }
        }
        return true;
    }

    bool FileSystem::ClearDir(const UString& path)
    {
        auto files = PlatformFile->QueryFiles(path, ".", false);
        //! files is BFS
        for (auto iter = files.rbegin(); iter != files.rend(); --iter)
        {
            if (IsDirectory(*iter) && !RemoveDir(*iter))
            {
                return false;
            }
            else if (IsFile(*iter) && !RemoveFile(*iter))
            {
                return false;
            }
        }
        return true;
    }

    DynamicArray<UString> FileSystem::QueryFiles(const UString& searchPath, const UString& regex, bool recursion)
    {
        return PlatformFile->QueryFiles(searchPath, regex, recursion);
    }

    UString FileSystem::GetEngineRootPath() const
    {
        return ENGINE_ROOT_PATH;
    }

    FileSystem::FileSystem()
    {
#if PLATFORM_WINDOWS
        PlatformFile = MakeUniquePtr<WindowsPlatformFile>();
#endif
    }

    void FileSystem::ReadFileToBinary(const UString& fileName, DynamicArray64<uint8>& outBinary)
    {
        UniquePtr<IFileHandle> handle = PlatformFile->OpenFile(fileName, EFileAccess::Read, EFileShareMode::Read);
        int64 fileSize = handle->GetSize();
        outBinary.Reserve(fileSize);
        handle->Read(outBinary.Data(), fileSize);
    }

    bool FileSystem::MakeFile(const UString& path)
    {
        return PlatformFile->MakeFile(path);
    }

    bool FileSystem::RemoveFile(const UString& path)
    {
        return PlatformFile->RemoveFile(path);
    }

    bool FileSystem::MoveFile(const UString& from, const UString& to)
    {
        return PlatformFile->MoveFile(from, to);
    }

    bool FileSystem::CopyFile(const UString& from, const UString& to)
    {
        return PlatformFile->CopyFile(from, to);
    }

    bool FileSystem::DirExists(const UString& path)
    {
        return PlatformFile->DirExists(path);
    }

    bool FileSystem::FileExists(const UString& path)
    {
        return PlatformFile->FileExists(path);
    }

    FileTime FileSystem::GetFileTime(const UString& path)
    {
        return PlatformFile->GetFileTime(path);
    }

    FileSystem::DirectoryIterImpl::DirectoryIterImpl(const UString& path)
    {
        FileHandle = MakeUniquePtr<PlatformFindFileHandle>(path);
    }

    bool FileSystem::DirectoryIterImpl::FindNext()
    {
        return (bool) FileHandle && FileHandle->FindNext(Entry);
    }

    FileSystem::DirectoryIterator::DirectoryIterator(const UString& path)
        : Impl(MakeSharedPtr<DirectoryIterImpl>(path))
    {}
}