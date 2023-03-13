#include "cxxopts.hpp"
#include "global.hpp"
#include "meta_parser.hpp"
#include "file_system/file_system.hpp"
#include "file_system/path.hpp"

using namespace Engine;

int main(int32 argc, char** argv)
{
    cxxopts::Options options("Moc", "Meta object compiler");

    options.add_options()
            ("s,standard", "cxx standard", cxxopts::value<int>()->default_value("11"))
            ("p,pch", "precompile header", cxxopts::value<std::string>());

    auto result = options.parse(argc, argv);

    ParserOptions parserOptions;
    parserOptions.Standard = result["standard"].as<int32>();
    if (result.count("pch"))
    {
        parserOptions.PrecompileHeader = String(result["pch"].as<std::string>().data());
    }

    MetaParser parser;
    parser.Parse(Path::Combine(FileSystem::GetEngineRootPath(), "test/core_test"), parserOptions);

    return 0;
}