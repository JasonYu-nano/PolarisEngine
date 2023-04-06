#include "cxxopts.hpp"
#include "meta_parser.hpp"
#include "file_system/file_system.hpp"
#include "file_system/path.hpp"
#include "meta_code_generator.hpp"
#include "moc_log.hpp"
#include "build_targets.hpp"

using namespace Engine;

int main(int32 argc, char** argv)
{
    cxxopts::Options options("Moc", "Meta object compiler");

    options.add_options()
            ("s,standard", "cxx standard", cxxopts::value<int>()->default_value("20"))
            ("p,pch", "precompile header", cxxopts::value<std::string>()->default_value("tools/meta_object_compiler/precompile.hpp"))
            ("c,content", "parse content", cxxopts::value<std::string>()->default_value("test/core_test"))
            ("i,include", "include dirs", cxxopts::value<std::string>()->default_value("C:/Code/PolarisEngine/engine/source/core/include,C:/Code/PolarisEngine/engine/source/core/include/global,C:/Code/PolarisEngine/engine/source/object/include"))
            ("t,target", "build target", cxxopts::value<std::string>()->default_value("core_test"));

    auto result = options.parse(argc, argv);

    ParserOptions parserOptions;
    parserOptions.Standard = result["standard"].as<int32>();
    parserOptions.PrecompileHeader = Path::Combine(FileSystem::GetEngineRootPath(), String(result["pch"].as<std::string>().data()));

    String contentOp = String::FromStdString(result["content"].as<std::string>());
    Array<String> contentArray = contentOp.Split(',');
    Set<String> contents;
    for (String& item : contentArray)
    {
        LOG_INFO(MOC, "Prepare parse content {}", item);
        contents.Add(Path::Combine(FileSystem::GetEngineRootPath(), item));
    }

    String includeOp = String::FromStdString(result["include"].as<std::string>());
    Array<String> includeDirs = includeOp.Split(',');
    for (String& dir : includeDirs)
    {
        parserOptions.IncludeDirs.Add(dir);
    }

    String targetOp = String::FromStdString(result["target"].as<std::string>());
    Array<String> buildTargets = targetOp.Split(',');
    for (String& target : buildTargets)
    {
        GBuildTargets.Add(target);
    }

    MetaParser parser;
    parser.Parse(contents, parserOptions);

    MetaCodeGenerator generator(parser);
    generator.GenerateCode();

    return 0;
}