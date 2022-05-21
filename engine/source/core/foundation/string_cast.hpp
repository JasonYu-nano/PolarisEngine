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

    template<typename InFromType, typename InToType = InFromType>
    class StringPointer
    {
        static_assert(sizeof(InFromType) == sizeof(InToType), "FromType must be the same size as ToType!");

    public:
        typedef InFromType FromType;
        typedef InToType   ToType;

    public:
        explicit StringPointer(const FromType* Source)
        {
            if (Source)
            {
                Ptr = (const ToType*)Source;
                StringLength = -1; // Len calculated on-demand
            }
            else
            {
                Ptr = nullptr;
                StringLength = 0;
            }
        }

        StringPointer(const FromType* Source, int32 SourceLen)
        {
            if (Source)
            {
                if (SourceLen > 0 && Source[SourceLen - 1] == 0)
                {
                    // Given buffer is null-terminated
                    SourceLen -= 1;
                }

                Ptr = (const ToType*)Source;
                StringLength = SourceLen;
            }
            else
            {
                Ptr = nullptr;
                StringLength = 0;
            }
        }

        /**
         * Move constructor
         */
        StringPointer(TStringPointer&& Other) = default;

        /**
         * Accessor for the string.
         * @note The string may not be null-terminated if constructed from an explicitly sized buffer that didn't include the null-terminator.
         *
         * @return A const pointer to the string.
         */
        FORCEINLINE const ToType* Get() const
        {
            return Ptr;
        }

        /**
         * Len of the string.
         *
         * @return The number of characters in the string, excluding any null terminator.
         */
        FORCEINLINE int32 Length() const
        {
            return StringLength == -1 ? TCString<ToType>::Strlen(Ptr) : StringLength;
        }

    private:
        // Non-copyable
        TStringPointer(const TStringPointer&) = delete;
        TStringPointer& operator=(const TStringPointer&) = delete;

        const ToType* Ptr;
        int32 StringLength;
    };


namespace Locale
{
    template <typename Source, typename Dest>
    static typename EnableIfT<!EncodingCompatible<Source, Dest>::Value, Dest*> Convert(Dest* dest, int32 destSize, const Source* src, int32 srcSize, Dest bogusChar = (Dest)'?')
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
            for (int I = 0; I < Length; ++I)
            {
                if (!CanConvertChar<Dest>(Src[I]))
                {
                    dest[I] = bogusChar;
                }
            }
        }*/

        return destSize < srcSize ? nullptr : dest + size;
    }

    const wchar* TempSCharToWChar(const ansi* str)
    {
        uint64 newSize = static_cast<uint64>(strlen(str)) + 1;
        wchar_t* wcstring = new wchar_t[newSize];
        size_t convertedChars = 0;
        mbstowcs_s(&convertedChars, wcstring, newSize, str, _TRUNCATE);
        return wcstring;
    }
}


    template<typename To, typename From>
    EnableIfT<EncodingCompatible<To, From>::Value, const To*> StringCast(const From* str)
    {
        return (const To*)str;
    }

    template<typename To, typename From>
    EnableIfT<!EncodingCompatible<To, From>::Value, const To*> StringCast(const From* str)
    {
        static_assert(false, "Unsupport encoding to cast");
        return nullptr;
    }

    const wchar* StringCast(const ansi* str)
    {
        uint64 newSize = static_cast<uint64>(strlen(str)) + 1;
        wchar_t* wcstring = new wchar_t[newSize];
        size_t convertedChars = 0;
        mbstowcs_s(&convertedChars, wcstring, newSize, str, _TRUNCATE);
        return wcstring;
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
