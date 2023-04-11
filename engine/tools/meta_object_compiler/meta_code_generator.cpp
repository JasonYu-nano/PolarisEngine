#include <fstream>
#include "meta_code_generator.hpp"
#include "file_system/path.hpp"
#include "file_system/file_system.hpp"
#include "moc_log.hpp"
#include "build_targets.hpp"

void MetaCodeGenerator::GenerateCode()
{
    Map<String, CodeWriter> writerMap;
    const String saveDir = Path::Combine(FileSystem::GetEngineIntermediateDir(), "generated/moc");
    if (!FileSystem::DirExists(saveDir))
    {
        FileSystem::MakeDirTree(saveDir);
    }

    for (auto&& unit : Parser)
    {
        const String targetSaveDir = Path::Combine(saveDir, unit->GetOwnerBuildTarget().ToString());
        if (!FileSystem::DirExists(targetSaveDir))
        {
            FileSystem::MakeDir(targetSaveDir);
        }

        const String fileName = Path::GetShortName(unit->GetFilePath(), false);

        String headerName = String::Format("{}.gen.hpp", fileName);
        String header = Path::Combine(targetSaveDir, headerName);
        String sourceName = String::Format("{}.gen.cpp", fileName);
        String source = Path::Combine(targetSaveDir, sourceName);

        writerMap.FindOrAdd(header, CodeWriter(headerName));
        writerMap.FindOrAdd(source, CodeWriter(sourceName));
        unit->GenerateCode(writerMap.FindRef(header), writerMap.FindRef(source));


        Set<String>& files = TargetDependenceFiles.FindOrAdd(unit->GetOwnerBuildTarget(), Set<String>());
        files.Add(unit->GetFilePath());
    }

    for (auto&& pair : writerMap)
    {
        if (pair.Value.Empty())
        {
            continue;
        }

        std::ofstream outFile(pair.Key.Data());
        outFile << pair.Value.GetCodeText().Data();
        outFile.flush();
        outFile.close();
        LOG_INFO(MOC, "Generate {}", pair.Key);
    }
}

void MetaCodeGenerator::GenerateManifest()
{
    for (auto&& pair : TargetDependenceFiles)
    {
        const StringID target = pair.Key;
        const Set<String> files = pair.Value;

        const String targetName = target.ToString();
        const String saveDir = Path::Combine(FileSystem::GetEngineIntermediateDir(), String::Format("generated/moc/{}", targetName)) ;
        ENSURE(FileSystem::DirExists(saveDir));

        const String manifest = Path::Combine(saveDir, "manifest.cmake");
        if (!FileSystem::FileExists(manifest))
        {
            FileSystem::MakeFile(manifest);
        }

        String manifestText = String::Format("set({}_manifest \n", targetName);
        for (auto&& file : files)
        {
            manifestText.Append(String::Format("\t{}\n", file));
        }
        manifestText.Append(")");

        std::ofstream outFile(manifest.Data());
        outFile << manifestText.Data();
        outFile.flush();
        outFile.close();

        LOG_INFO(MOC, "Generate {}", manifest);
    }
}
