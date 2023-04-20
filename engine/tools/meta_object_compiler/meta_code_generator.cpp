#include <fstream>
#include "meta_code_generator.hpp"
#include "file_system/path.hpp"
#include "file_system/file_system.hpp"
#include "moc_log.hpp"
#include "build_targets.hpp"

void MetaCodeGenerator::Generate()
{
    GenerateCode();
    GenerateManifest();
}

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
        StringID buildTarget = unit->GetOwnerBuildTarget();
        if (buildTarget == STRING_ID_NONE)
        {
            LOG_INFO(MOC, "Header file: {} belongs to an invalid target", unit->GetFilePath());
            continue;
        }

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


        Set<String>& files = TargetDependenceFiles.FindOrAdd(buildTarget, Set<String>());
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
        Set<String> files = pair.Value;

        const String targetName = target.ToString();
        const String saveDir = Path::Combine(FileSystem::GetEngineIntermediateDir(), String::Format("generated/moc/{}", targetName)) ;
        ENSURE(FileSystem::DirExists(saveDir));

        const String manifest = Path::Combine(saveDir, "manifest.moc");
        if (!FileSystem::FileExists(manifest))
        {
            FileSystem::MakeFile(manifest);
        }

        if (GIncrementalBuild)
        {
            Array<String> manifestItems;
            CleanupManifest(manifest, &manifestItems);
            MergeManifestItem(files, manifestItems);
        }

        String manifestText;
        bool isFirstItem = true;
        for (auto&& file : files)
        {
            isFirstItem ? manifestText.Append(file) : manifestText.Append('\n').Append(file);
            isFirstItem = false;
        }

        std::ofstream outFile(manifest.Data());
        outFile << manifestText.Data();
        outFile.flush();
        outFile.close();

        LOG_INFO(MOC, "Generate {}", manifest);

        CleanupExpiredGeneratedFiles(saveDir, files);
    }
}

void MetaCodeGenerator::CleanupManifest(const String& manifest, Array<String>* validManifestItems)
{
    Array<String> items;
    GetManifestItem(manifest, items);
    for (auto it = items.begin(); (bool)it; ++it)
    {
        if (!FileSystem::FileExists(*it))
        {
            it.RemoveSelf();
        }
    }

    if (validManifestItems)
    {
        *validManifestItems = std::move(items);
    }
}

void MetaCodeGenerator::MergeManifestItem(Set<String>& files, const Array<String>& items)
{
    for (auto&& item : items)
    {
        files.Add(item);
    }
}

void MetaCodeGenerator::GetManifestItem(const String& manifest, Array<String>& manifestItems)
{
    if (!FileSystem::FileExists(manifest))
    {
        return;
    }

    String fileText = FileSystem::ReadFileToString(manifest);
    manifestItems = fileText.Split('\n');
}

void MetaCodeGenerator::CleanupExpiredGeneratedFiles(const String& path, const Set<String>& manifestItems)
{
    Array<String> files = FileSystem::QueryFiles(path, "\\w+\\.gen\\.\\w+");

    Set<String> shortNameItems(manifestItems.Size());
    for (const String& item : manifestItems)
    {
        shortNameItems.Add(Path::GetShortName(item, false));
    }

    for (auto&& file : files)
    {
        String shortFileName = Path::GetShortName(file);
        shortFileName.Remove(shortFileName.Length() - 8, 8);
        if (!shortNameItems.Contains(shortFileName))
        {
            FileSystem::RemoveFile(file);
        }
    }
}