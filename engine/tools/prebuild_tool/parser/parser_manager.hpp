#pragma once

#include "parser/reflection_unit.hpp"
#include <future>

namespace Engine
{
    class ParserManager
    {
    public:
        static ParserManager& Get();

        /**
         * Parse all header in specifier content.
         * Async, @see WaitForComplete
         * @param dir
         * @param args
         * @param pchPath
         */
        void ParseContent(const String& dir, DynamicArray<const ansi*> args = {}, const String& pchPath = String::Empty());

        /**
         * Yield and wait for all reflection unit parse complete
         */
        void WaitForComplete();

    private:
        String GeneratePCH(const String& filePath);

        void DeserializeModifyTimeMap();

        void SerializeModifyTimeMap();

    private:
        DynamicArray<std::future<UniquePtr<ReflectionUnit>>> ReflectionUnitFutures;
        DynamicArray<UniquePtr<ReflectionUnit>> ReflectionUnits;
        std::unordered_map<String, size_t> FileLastModifyTimeMap;
    };
}