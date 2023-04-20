#pragma once

#include "meta_parser.hpp"
#include "code_generator_interface.hpp"

namespace Engine
{
    class MetaCodeGenerator
    {
    public:
        MetaCodeGenerator(const MetaParser& parser) : Parser(parser) {}

        void Generate();

    private:
        void GenerateCode();

        void GenerateManifest();

        void CleanupManifest(const String& manifest, Array<String>* validManifestItems = nullptr);

        void MergeManifestItem(Set<String>& files, const Array<String>& items);

        void GetManifestItem(const String& manifest, Array<String>& manifestItems);

        void CleanupExpiredGeneratedFiles(const String& path, const Set<String>& manifestItems);
    private:
        const MetaParser& Parser;
        Map<StringID, Set<String>> TargetDependenceFiles;
    };
}
