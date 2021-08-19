#pragma once
#include "foundation/type_traits.hpp"
#include <stdlib.h>

namespace Engine
{
#ifdef UNICODE
#define SCHAR_TO_TCHAR(str) Locale::TempSCharToWChar(str)
#else
#define SCHAR_TO_TCHAR(str) str
#endif


    template<typename EncodingA, typename EncodingB>
    struct EncodingCompatible
    {
        bool Value = sizeof(EncodingA) == sizeof(EncodingB);
    };


namespace Locale
{
    template <typename Source, typename Dest>
    static typename TEnableIf<!EncodingCompatible<Source, Dest>::Value, Dest*> Convert(Dest* dest, int32 destSize, const Source* src, int32 srcSize, Dest bogusChar = (Dest)'?')
    {
        const int32 size = destSize <= srcSize ? destSize : srcSize;
        bool invalidChars = false;
        for (int i = 0; i < size; ++i)
        {
            Source SrcCh = src[i];
            dest[i] = (Dest)SrcCh;
            //bInvalidChars |= !CanConvertChar<Dest>(SrcCh);
        }

        /*if (invalidChars)
        {
            for (int I = 0; I < Size; ++I)
            {
                if (!CanConvertChar<Dest>(Src[I]))
                {
                    dest[I] = bogusChar;
                }
            }
        }*/

        return destSize < srcSize ? nullptr : dest + size;
    }

    const wchar* TempSCharToWChar(const schar* str)
    {
        uint64 newSize = static_cast<uint64>(strlen(str)) + 1;
        wchar_t* wcstring = new wchar_t[newSize];
        size_t convertedChars = 0;
        mbstowcs_s(&convertedChars, wcstring, newSize, str, _TRUNCATE);
        return wcstring;
    }
}


    template<typename To, typename From>
    TEnableIf<EncodingCompatible<To, From>::Value, const To*> StringCast(const From* str)
    {
        return (const To*)str;
    }

    template<typename To, typename From>
    TEnableIf<!EncodingCompatible<To, From>::Value, const To*> StringCast(const From* str)
    {
        return (const To*)str;
    }

    template <typename From, typename To>
    struct StringConvert
    {
        static void Convert(To* dest, int32 destLen, const From* source, int32 sourceLen)
        {
            To* result = Locale::Convert(dest, destLen, source, sourceLen);
        }
    };
}
