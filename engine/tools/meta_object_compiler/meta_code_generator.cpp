#include <fstream>
#include "meta_code_generator.hpp"
#include "file_system/path.hpp"
#include "file_system/file_system.hpp"

void MetaCodeGenerator::GenerateCode()
{
    Map<String, CodeWriter> writerMap;
    String saveDir = Path::Combine(FileSystem::GetEngineIntermediateDir(), "generated/moc") ;
    for (auto&& unit : Parser)
    {
        String fileName = Path::GetShortName(unit->GetFilePath(), false);
        String outputFile = Path::Combine(saveDir, String::Format("{}.gen.hpp", fileName));

        CodeWriter& writer = writerMap.FindOrAdd(outputFile, CodeWriter());
        unit->GenerateCode(writer);
    }

    if (!FileSystem::DirExists(saveDir))
    {
        FileSystem::MakeDirTree(saveDir);
    }

    for (auto&& pair : writerMap)
    {
        std::ofstream outFile(pair.Key.Data());
        outFile << pair.Value.GetCodeText().Data();
        outFile.close();
    }
}
