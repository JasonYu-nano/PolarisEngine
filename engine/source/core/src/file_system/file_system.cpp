//#include "precompiled_core.hpp"
#include "file_system/file_system.hpp"
#include "file_system/path.hpp"

#if PLATFORM_WINDOWS
#include "windows/windows_platform_file.hpp"
#include "windows/windows_file_handle.hpp"
#endif

namespace Engine
{
    bool FileSystem::MakeDir(const String& path)
    {
        return PlatformFile->MakeDir(path);
    }

    bool FileSystem::RemoveDir(const String& path)
    {
        return PlatformFile->RemoveDir(path);
    }

    bool FileSystem::MakeDirTree(const String& path)
    {
        Array<String> dirs = Path::SplitPath(path);
        if (dirs.Size() <= 0)
        {
            return false;
        }

        String destPath;
        for (int32 index = 0; index < dirs.Size(); index++)
        {
            destPath = Path::Combine(destPath, dirs[index]);
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

    bool FileSystem::ClearDir(const String& path)
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

    Array<String> FileSystem::QueryFiles(const String& searchPath, const String& regex, bool recursion)
    {
        return PlatformFile->QueryFiles(searchPath, regex, recursion);
    }

    String FileSystem::GetEngineRootPath()
    {
        return ENGINE_ROOT_PATH;
    }

#if PLATFORM_WINDOWS
    UniquePtr<IPlatformFile> FileSystem::PlatformFile = MakeUnique<WindowsPlatformFile>();
#endif

    void FileSystem::ReadFileToBinary(const String& fileName, Array64<uint8>& outBinary)
    {
        UniquePtr<IFileHandle> handle = PlatformFile->OpenFile(fileName, EFileAccess::Read, EFileShareMode::Read);
        int64 fileSize = handle->GetSize();
        outBinary.Resize(fileSize);
        handle->Read(outBinary.Data(), fileSize);
    }

    bool FileSystem::MakeFile(const String& path)
    {
        return PlatformFile->MakeFile(path);
    }

    bool FileSystem::RemoveFile(const String& path)
    {
        return PlatformFile->RemoveFile(path);
    }

    bool FileSystem::MoveFile(const String& from, const String& to)
    {
        return PlatformFile->MoveFile(from, to);
    }

    bool FileSystem::CopyFile(const String& from, const String& to)
    {
        return PlatformFile->CopyFile(from, to);
    }

    bool FileSystem::DirExists(const String& path)
    {
        return PlatformFile->DirExists(path);
    }

    bool FileSystem::FileExists(const String& path)
    {
        return PlatformFile->FileExists(path);
    }

    FileTime FileSystem::GetFileTime(const String& path)
    {
        return PlatformFile->GetFileTime(path);
    }

    FileSystem::DirectoryIterImpl::DirectoryIterImpl(const String& path, bool recursive)
    {
        if (recursive)
        {
            FileHandle = MakeUnique<PlatformRecursiveFindFileHandle>(path);
        }
        else
        {
            FileHandle = MakeUnique<PlatformFindFileHandle>(path);
        }
    }

    bool FileSystem::DirectoryIterImpl::FindNext()
    {
        return (bool) FileHandle && FileHandle->FindNext(Entry);
    }

    FileSystem::DirectoryIterator::DirectoryIterator(const String& path, bool recursive)
        : Impl(MakeShared<DirectoryIterImpl>(path, recursive))
    {
        if (!Impl->FindNext())
        {
            Impl.reset();
        }
    }
}