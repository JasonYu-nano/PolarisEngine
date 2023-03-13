#include <iostream>
#include <clang-c/Index.h>
#include "meta_parser.hpp"
#include "file_system/path.hpp"
#include "file_system/file_system.hpp"
#include "class_unit.hpp"
#include "enum_unit.hpp"
#include "parse_utils.hpp"

namespace Engine
{
    void MetaParser::Parse(const String& content, const ParserOptions& options)
    {
        String normalizedContent = Path::Normalize(content);
        auto headFiles = FileSystem::QueryFiles(normalizedContent, ".*.hpp", true);
        Array<const char*> parserArgs = {"-xc++",
                                         "-DMOC_BUILDER",
                                         "-IC:/Code/PolarisEngine/engine/source/core/include",
                                         String::Format("-std={0}", options.Standard).Data()};

        if (options.PrecompileHeader.has_value())
        {
            const String& pch = options.PrecompileHeader.value();
            String clpch = GeneratePrecompileHeader(pch);
            parserArgs.Add(String::Format("-include-pch{0}", clpch).Data());
        }

        for (auto&& header : headFiles)
        {
            ParseImpl(header, parserArgs);
        }
    }

    void MetaParser::ParseImpl(const String& header, const Array<const char*>& args)
    {
        CXIndex cxIndex = clang_createIndex(true, false);
        CXTranslationUnit tu;

        auto error = clang_parseTranslationUnit2(
            cxIndex,
            header.Data(),
            args.Data(),
            args.Size(),
            nullptr,
            0,
            CXTranslationUnit_None,
            &tu
        );

        if (error != CXError_Success)
        {
            return;
        }

        CXCursor cursor = clang_getTranslationUnitCursor(tu);

        for (auto&& child : ParseUtils::GetCursorChildren(cursor))
        {
            switch (clang_getCursorKind(child))
            {
                case CXCursor_EnumDecl:
                {
                    if (ParseUtils::CursorHasMeta(child))
                    {
                        EnumUnit unit(child, header);
                        unit.Parse();
                    }
                    break;
                }
                case CXCursor_StructDecl:
                {
                    if (ParseUtils::CursorHasMeta(child))
                    {

                    }
                    break;
                }
                case CXCursor_ClassDecl:
                {
                    if (ParseUtils::CursorHasMeta(child))
                    {
                        ClassUnit unit(child, header);
                        unit.Parse();
                    }
                    break;
                }
                default:
                {
                    // do nothing
                }
            }
        }

        clang_disposeTranslationUnit(tu);
    }

    String MetaParser::GeneratePrecompileHeader(const String& pch)
    {
        if (!FileSystem::FileExists(pch))
        {
            std::cerr << "precompile header not exists !" << std::endl;
            return "";
        }

        String savePath = Path::Combine(FileSystem::GetEngineIntermediateDir(), "moc/pch");
        if (!FileSystem::DirExists(savePath) && !FileSystem::MakeDirTree(savePath))
        {
            std::cerr << "make pch output dir failed !" << std::endl;
            return "";
        }

        String fileName = Path::GetShortName(pch, false);
        String savedFile = Path::Combine(savePath, Path::RemoveExtension(fileName) + ".pch");

        if (FileSystem::FileExists(savedFile))
        {
            FileTime headerFTime = FileSystem::GetFileTime(pch);
            FileTime pchFTime = FileSystem::GetFileTime(savedFile);
            if (headerFTime.LastModifyTime <= pchFTime.LastModifyTime)
            {
                return savedFile;
            }
        }

        CXIndex cxIdx = clang_createIndex(true, false);
        CXTranslationUnit tu = clang_parseTranslationUnit(
            cxIdx,
            pch.Data(),
            nullptr,
            0,
            nullptr,
            0,
            CXTranslationUnit_ForSerialization
        );
        clang_saveTranslationUnit(tu, pch.Data(), clang_defaultSaveOptions(tu));

        return savedFile;
    }
}