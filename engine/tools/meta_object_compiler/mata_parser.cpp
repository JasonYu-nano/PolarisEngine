#include <clang-c/Index.h>
#include "meta_parser.hpp"
#include "file_system/path.hpp"
#include "file_system/file_system.hpp"
#include "class_unit.hpp"
#include "enum_unit.hpp"
#include "parse_utils.hpp"
#include "moc_log.hpp"
#include "struct_unit.hpp"
#include "build_targets.hpp"

namespace Engine
{
    MetaParser::~MetaParser()
    {
        for (auto&& unit : Units)
        {
            delete unit;
        }

        Units.Clear();
    }

    void MetaParser::Parse(const Set<String>& content, const ParserOptions& options)
    {
        // Record parse time
        ParseTime = PlatformClock::Now().TimeSinceEpoch();

        DisplayDiagnostics = options.DisplayDebugInfo;

        String standardOp = String::Format("-std=c++{0}", options.Standard);
        Array<const char*> parserArgs = {"-xc++",
                                         "-DMOC_BUILDER",
                                         "-DPL_SHARED",
                                         "-DNDEBUG",
                                         "-D__clang__",
                                         standardOp.Data()};

        Array<String> includeDirOpts;
        for (auto&& dir : options.IncludeDirs)
        {
            includeDirOpts.Add(String::Format("-I{}", dir));
        }

        for (auto&& dir : includeDirOpts)
        {
            parserArgs.Add(dir.Data());
        }
//        FIXME: Precompile has some error
//        String pchInclude = "-include-pch";
//        String pchFile = GeneratePrecompileHeader(options.PrecompileHeader);
//        if (!pchFile.Empty())
//        {
//            parserArgs.Add(pchInclude.Data());
//            parserArgs.Add(pchFile.Data());
//        }

        int64 lastParseTime = GetLastParseTime();
        for (auto&& dir : content)
        {
            String normalizedDir = Path::Normalize(dir);
            auto headFiles = FileSystem::QueryFiles(normalizedDir, ".*.hpp", true);

            for (auto&& header : headFiles)
            {
                if (GIncrementalBuild)
                {
                    FileTime fileTime = FileSystem::GetFileTime(header);
                    if (static_cast<int64>(fileTime.LastModifyTime) < lastParseTime)
                    {
                        continue;
                    }
                }

                ParseImpl(header, parserArgs);
            }
        }

        UpdateLastParseTime(ParseTime);
    }

    void MetaParser::ParseImpl(const String& header, const Array<const char*>& args)
    {
        String text = FileSystem::ReadFileToString(header);
        if (!text.Contains("META"))
        {
            return;
        }

        CXIndex cxIndex = clang_createIndex(true, DisplayDiagnostics ? 1 : 0);
        CXTranslationUnit tu;

        auto error = clang_parseTranslationUnit2(
            cxIndex,
            header.Data(),
            args.Data(),
            args.Size(),
            nullptr,
            0,
            CXTranslationUnit_None|CXTranslationUnit_SkipFunctionBodies,
            &tu
        );

        if (error != CXError_Success)
        {
            LOG_ERROR(MOC, "Parse header: {} failed, error code: {}", header, error);
            return;
        }

        CXCursor cursor = clang_getTranslationUnitCursor(tu);

        ParseCursor(cursor);

        clang_disposeTranslationUnit(tu);
        clang_disposeIndex(cxIndex);
    }

    void MetaParser::ParseCursor(const CXCursor& cursor, const String& nameSpace)
    {
        for (auto&& child : ParseUtils::GetCursorChildren(cursor))
        {
            RecordUnit* unit = nullptr;

            switch (clang_getCursorKind(child))
            {
                case CXCursor_EnumDecl:
                {
                    if (ParseUtils::CursorHasMeta(child) && !IsCursorParsed(child))
                    {
                        CX_TOSTRING_EXPR(enumName, clang_getCursorSpelling(child))
                        LOG_INFO(MOC, "Start parse enum: {}", enumName);
                        unit = new EnumUnit(child, nameSpace);
                    }
                    break;
                }
                case CXCursor_StructDecl:
                {
                    if (ParseUtils::CursorHasMeta(child) && !IsCursorParsed(child))
                    {
                        CX_TOSTRING_EXPR(structName, clang_getCursorSpelling(child))
                        LOG_INFO(MOC, "Start parse struct: {}", structName);
                        unit = new StructUnit(child, nameSpace);
                    }
                    break;
                }
                case CXCursor_ClassDecl:
                {
                    if (ParseUtils::CursorHasMeta(child) && !IsCursorParsed(child))
                    {
                        CX_TOSTRING_EXPR(className, clang_getCursorSpelling(child))
                        LOG_INFO(MOC, "Start parse class: {}", className);
                        unit = new ClassUnit(child, nameSpace);
                    }
                    break;
                }
                case CXCursor_Namespace:
                {
                    CX_TOSTRING_EXPR(ns, clang_getCursorSpelling(child))
                    if (ns != "std")
                    {
                        String newNameSpace = nameSpace;
                        if (newNameSpace.Empty())
                        {
                            newNameSpace.Append(ns);
                        }
                        else
                        {
                            newNameSpace.Append("::").Append(ns);
                        }
                        ParseCursor(child, newNameSpace);
                    }
                    break;
                }
                default:
                {
                }
            }

            if (unit)
            {
                unit->Parse();
                Units.Add(unit);
                ParsedMetaObjectNames.Add(unit->GetRecordName());
            }
        }
    }

    String MetaParser::GeneratePrecompileHeader(const String& pch)
    {
        if (!FileSystem::FileExists(pch))
        {
            LOG_WARN(MOC, "Precompile header not exists !");
            return "";
        }

        String savePath = Path::Combine(FileSystem::GetEngineIntermediateDir(), "moc/pch");
        if (!FileSystem::DirExists(savePath) && !FileSystem::MakeDirTree(savePath))
        {
            LOG_WARN(MOC, "Make pch output dir failed !");
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
        clang_saveTranslationUnit(tu, savedFile.Data(), clang_defaultSaveOptions(tu));

        return savedFile;
    }

    bool MetaParser::IsCursorParsed(CXCursor cursor) const
    {
        CX_TOSTRING_EXPR(cursorName, clang_getCursorSpelling(cursor))
        return ParsedMetaObjectNames.Contains(cursorName);
    }

    const String& GetMocConfigPath()
    {
        static String path = Path::Combine(FileSystem::GetEngineIntermediateDir(), "moc/config.toml");
        return path;
    }

    toml::table GetMocConfig()
    {
        const String& path = GetMocConfigPath();
        if (FileSystem::FileExists(path))
        {
            toml::parse_result result = toml::parse_file(path.Data());
            if (result)
            {
                return result.table();
            }
        }

        static constexpr auto source = R"(
            [parser]
            LastParseTime = 0
        )";

        toml::parse_result result = toml::parse(source);
        ENSURE(result);

        return result.table();
    }

    int64 MetaParser::GetLastParseTime() const
    {
        Config = GetMocConfig();
        std::optional<int32> lastParseTime = Config["parser"]["LastParseTime"].value<int32>();
        return lastParseTime.has_value() ? lastParseTime.value() : 0;
    }

    void MetaParser::UpdateLastParseTime(int64 parseTime) const
    {
        auto view = Config["parser"]["LastParseTime"];
        if (view.is_number())
        {
            int64& time = view.ref<int64>();
            time = parseTime;
            std::ofstream stream(GetMocConfigPath().Data());
            stream << Config;
            stream.flush();
            stream.close();
        }
    }
}