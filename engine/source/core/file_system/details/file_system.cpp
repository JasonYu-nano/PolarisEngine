//#include "precompiled_core.hpp"
#include "file_system/file_system.hpp"
#include "file_system/path.hpp"

#if PLATFORM_WINDOWS
#include "windows/windows_platform_file.hpp"
#endif

namespace Engine
{
    FileSystem& FileSystem::Get()
    {
        static FileSystem instance;
        return instance;
    }

    bool FileSystem::MakeDir(const String& path)
    {
        return PlatformFile->MakeDir(*path);
    }

    bool FileSystem::RemoveDir(const String& path)
    {
        return PlatformFile->RemoveDir(*path);
    }

    bool FileSystem::MakeFile(const String& path)
    {
        return PlatformFile->MakeFile(*path);
    }

    inline bool FileSystem::RemoveFile(const String& path)
    {
        return PlatformFile->RemoveFile(*path);
    }

    bool FileSystem::DirExists(const String& path)
    {
        return PlatformFile->DirExists(*path);
    }

    bool FileSystem::FileExists(const String& path)
    {
        return PlatformFile->FileExists(*path);
    }

    bool FileSystem::IsDirectory(const String& path)
    {
        return PlatformFile->DirExists(*path);
    }

    bool FileSystem::IsFile(const String& path)
    {
        return PlatformFile->FileExists(*path);
    }

    bool FileSystem::MakeDirTree(const String& path)
    {
        DynamicArray<String> dirs = Path::SplitPath(path);
        if (dirs.Size() <= 0)
        {
            return false;
        }
        
        String destPath = String::Empty();
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

    bool FileSystem::ClearDir(const String& path)
    {
        auto files = PlatformFile->QueryFiles(*path, _T("."), false);
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

    FileTime Engine::FileSystem::GetFileTime(const String& path)
    {
        return PlatformFile->GetFileTime(*path);
    }

    DynamicArray<String> FileSystem::QueryFiles(const String& searchPath, const String& regex, bool recursion)
    {
        return PlatformFile->QueryFiles(*searchPath, *regex, recursion);
    }

    String FileSystem::GetEngineRootPath() const
    {
        return ENGINE_ROOT_PATH;
    }

    FileSystem::FileSystem()
    {
#if PLATFORM_WINDOWS
        PlatformFile = MakeUniquePtr<WindowsPlatformFile>();
#endif
    }

    void FileSystem::ReadFileToBinary(const String& fileName, DynamicArrayU64<uint8>& outBinary)
    {
        UniquePtr<IFileHandle> handle = PlatformFile->OpenFile(*fileName, EFileAccess::Read, EFileShareMode::Read);
        uint64 fileSize = handle->GetSize();
        outBinary.Reserve(fileSize);
        handle->Read(outBinary.Data(), fileSize);
    }
}