#include "core_minimal_public.hpp"
#include "parser/parser_manager.hpp"
#include "gflags/gflags.h"

using namespace Engine;

DEFINE_string(std, "c++17", "stl version");
DEFINE_string(contentPath, "", "parser content path");
DEFINE_string(outputDir, "", "direction of output generated file");
DEFINE_string(pch, "", "path of pch");

int32 main(int32 argv, ansi** args)
{
//    const ansi* std = FLAGS_std.c_str();
//    const ansi* contentPath = FLAGS_contentPath.c_str();
//    const ansi* pch = FLAGS_pch.c_str();
//    const ansi* outputDir = FLAGS_outputDir.c_str();

    const ansi* std = "c++21";
    const ansi* contentPath = "C:/Code/PolarisEngine/engine/source/core";
    const char_t* pch = _T("C:/Code/PolarisEngine/engine/source/core/precompiled_core.hpp");
    const ansi* outputDir = "C:/Code/PolarisEngine/engine/intermediate/generated/core";

    DynamicArray<const ansi*> parseArgs = {"-xc++", Format("-std={0}", std).data()};

    ParserManager::Get().ParseContent(A2W(contentPath), parseArgs, pch);

    ParserManager::Get().WaitForComplete();
    return 0;
}