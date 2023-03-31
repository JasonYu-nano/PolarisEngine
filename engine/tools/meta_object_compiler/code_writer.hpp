#pragma once

#include "foundation/string.hpp"

namespace Engine
{
    class CodeWriter
    {
    public:
        bool Empty() const
        {
            return CodeText.Empty();
        }

        const String& GetCodeText() const
        {
            return CodeText;
        }

        void WriteLine(String&& text)
        {
            CodeText.Reserve(CodeText.Length() + text.Length() + NumTab + 2);
            for (int32 i = 0; i < NumTab; ++i)
            {
                CodeText.Append('\t');
            }
            CodeText.Append(text).Append('\n');
        }

        void WriteEmptyRow()
        {
            CodeText.Append('\n');
        }

        void AddTab()
        {
            ++NumTab;
        }

        void RemoveTab()
        {
            if (NumTab > 0)
            {
                --NumTab;
            }
        }

    private:
        int32 NumTab{ 0 };
        String CodeText;
    };
}
