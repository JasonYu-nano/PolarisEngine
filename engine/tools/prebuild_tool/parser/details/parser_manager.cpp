#include <iostream>
#include <fstream>
#include "parser/parser_manager.hpp"
#include "file_system/file_system.hpp"
#include "file_system/path.hpp"
#include "clang-c/Index.h"
#include "nlohmann/json.hpp"

namespace Engine
{

    ParserManager& ParserManager::Get()
    {
        static ParserManager inst;
        return inst;
    }

    void ParserManager::ParseContent(const String& dir, DynamicArray<const ansi*> args, const String& pchPath)
    {
        DeserializeModifyTimeMap();

        if (pchPath != String::Empty())
        {
            String genPCHPath = GeneratePCH(pchPath);
            if (genPCHPath != String::Empty())
            {
                args.Add(Format("-include-pch{0}", (const ansi*)genPCHPath).data());
            }
        }

        auto headFiles = FileSystem::Get().QueryFiles(dir, _T(".*.hpp"), true);

        for (auto&& header : headFiles)
        {
            FileTime headerFTime = FileSystem::Get().GetFileTime(header);
            auto iter = FileLastModifyTimeMap.find(header);
            if (iter != FileLastModifyTimeMap.end() && iter->second >= headerFTime.LastModifyTime)
            {
                continue;
            }

            std::future<UniquePtr<ReflectionUnit>> unit = std::async(std::launch::async|std::launch::deferred, [header, args](){
                UniquePtr<ReflectionUnit> unit = ReflectionUnit::CreateUnit(header, args);
                return unit;
            });

            ReflectionUnitFutures.Add(MoveTemp(unit));

            std::chrono::time_point timePoint = std::chrono::system_clock::now();
            auto sec = duration_cast<std::chrono::seconds>(timePoint.time_since_epoch());
            FileLastModifyTimeMap.insert({header, sec.count()});
        }
    }

    void ParserManager::WaitForComplete()
    {
        for (auto&& unitFuture : ReflectionUnitFutures)
        {
            UniquePtr<ReflectionUnit> unit = unitFuture.get();
            if (unit)
            {
                ReflectionUnits.Add(MoveTemp(unit));
            }
        }

        SerializeModifyTimeMap();
    }

    String ParserManager::GeneratePCH(const String& filePath)
    {
        if (!FileSystem::Get().FileExists(filePath))
        {
            std::cerr << "precompile header not exists !" << std::endl;
            return String::Empty();
        }

        String savePath = Path::MakePath(FileSystem::Get().GetEngineRootPath(), _T("intermediate/clang_pch"));
        if (!FileSystem::Get().DirExists(savePath) && !FileSystem::Get().MakeDirTree(savePath))
        {
            std::cerr << "make pch output dir failed !" << std::endl;
            return String::Empty();
        }
        String fileName = Path::GetShortName(filePath, false);
        String saveFile = Path::MakePath(savePath, Path::RemoveExtension(fileName) + _T(".pch"));

        if (FileSystem::Get().FileExists(saveFile))
        {
            FileTime headerFTime = FileSystem::Get().GetFileTime(filePath);
            FileTime pchFTime = FileSystem::Get().GetFileTime(saveFile);
            if (headerFTime.LastModifyTime <= pchFTime.LastModifyTime)
            {
                return saveFile;
            }
        }

        CXIndex cxIdx = clang_createIndex(true, false);
        CXTranslationUnit tu = clang_parseTranslationUnit(
            cxIdx,
            (const char*)filePath,
            nullptr,
            0,
            nullptr,
            0,
            CXTranslationUnit_ForSerialization
        );
        clang_saveTranslationUnit(tu, (const char*)saveFile, clang_defaultSaveOptions(tu));

        return saveFile;
    }

    void ParserManager::DeserializeModifyTimeMap()
    {
        String filePath = Path::MakePath(FileSystem::Get().GetEngineRootPath(), _T("/intermediate/prebuild_tool/parser_file_config.json"));
        if (!FileSystem::Get().FileExists(filePath))
        {
            return;
        }

        std::ifstream ifs;
        ifs.open(*filePath, std::ios::in);

        using namespace nlohmann;
        json j;
        ifs >> j;
        ifs.close();
        FileLastModifyTimeMap = j.get<std::unordered_map<String, size_t>>();
    }

    void ParserManager::SerializeModifyTimeMap()
    {
        String fileDir = Path::MakePath(FileSystem::Get().GetEngineRootPath(), _T("/intermediate/prebuild_tool"));
        if (!FileSystem::Get().DirExists(fileDir) && !FileSystem::Get().MakeDirTree(fileDir))
        {
            std::cerr << "create parser_file_config.json failed" << std::endl;
            return;
        }

        String filePath = Path::MakePath(fileDir, _T("/parser_file_config.json"));
        if (!FileSystem::Get().FileExists(filePath) && !FileSystem::Get().MakeFile(filePath))
        {
            std::cerr << "create parser_file_config.json failed" << std::endl;
            return;
        }

#ifdef UNICODE
        std::ofstream ofs;
        ofs.open(*filePath, std::ios::out);
#else
        std::ofstream ofs;
        ofs.open(fileDir, std::ios::out);
#endif

        using namespace nlohmann;

        json j = FileLastModifyTimeMap;
        ofs << std::setw(4) << j << std::endl;
        ofs.close();
    }
}