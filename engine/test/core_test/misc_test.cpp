#include "gtest/gtest.h"
#include "core_minimal_public.hpp"
#include "math/city_hash.hpp"
#include "foundation/fixed_string.hpp"
#include "file_system/path.hpp"
#include "file_system/file_system.hpp"
#include "foundation/string.hpp"

namespace Engine
{
    TEST(Codecvt, All)
    {
        const char* a = "你好";
        EXPECT_TRUE(Utf8::ToUCS4(a + 0, a, a + 6) == 0x4F60);
        EXPECT_TRUE(Utf8::ToUCS4(a + 1, a, a + 6) == 0x4F60);
        EXPECT_TRUE(Utf8::ToUCS4(a + 2, a, a + 6) == 0x4F60);
    }

    TEST(Path, All)
    {
        String path = "c:/dirA\\dirB/file.ex";
        String ex = Path::GetShortName(path, false);
        EXPECT_TRUE(Path::GetExtension(path) == ".ex");
        EXPECT_TRUE(Path::GetShortName(path, false) == "file");
        EXPECT_TRUE(Path::SplitPath(path).Size() == 4);
    }

    TEST(FixedString, Base)
    {
        FixedString name("Hello_World");
        EXPECT_TRUE(name.ToString() == "Hello_World");

        FixedString name2("Hello_World_12");
        EXPECT_TRUE(name2.ToString() == "Hello_World_12" && name2.GetNumber() == 12);

        FixedString name3("Hello_World_012");
        EXPECT_TRUE(name3.GetNumber() == 0);

        FixedString name4("hello_World_12");
        EXPECT_TRUE(name2 == name4);
        EXPECT_TRUE(name4.ToString() == "Hello_World_12");
    }

//    TEST(UChar, Base)
//    {
//        UChar ch('A');
//        ch.ToLower();
//        EXPECT_TRUE(ch == 'a');
//    }

    TEST(StringView, Ctor)
    {
        StringView view = "abcd1234";
        EXPECT_TRUE(view.StartsWith("ab"));
        EXPECT_TRUE(view.EndsWith("234"));
        EXPECT_TRUE(view.IndexOf("d1") == 3);
        EXPECT_FALSE(view.Contains("d2"));

        view = "am21nightamdayam";
        auto ret = view.Split("am");
        EXPECT_TRUE(ret.Size() == 4);
    }

//    TEST(UString, Ctor)
//    {
//        UString str = UString('a');
//        EXPECT_TRUE(str[0] == 'a');
//
//        str = UString::FromLatin1("1234\0");
//        EXPECT_TRUE(str[0] == '1' && str[1] == '2' && str[2] == '3' && str[3] == '4');
//
//        str = UString::FromUtf8("你好");
//        UString str2 = UString::FromStdString("你好");
//        EXPECT_TRUE(str == str2);
//
//        auto u8 = str.ToUtf8();
//        EXPECT_TRUE(UString::FromUtf8(u8.Data()) == str);
//
//        EXPECT_TRUE(str.ToStdString() == u8.Data());
//    }

    struct TestChar
    {
        TestChar() {};

        TestChar(char c) : Data(c) {};

        ~TestChar()
        {
            Data = '0';
        }

        friend bool operator== (const TestChar& lhs, const TestChar& rhs)
        {
            return lhs.Data == rhs.Data;
        }

        friend bool operator!= (const TestChar& lhs, const TestChar& rhs)
        {
            return lhs.Data != rhs.Data;
        }

        char Data{ '\0' };
    };

    template <>
    struct CharTraits<TestChar> : PrivateCharTraits<TestChar, int8> {};

    using TestString = BasicString<TestChar>;

    TEST(TestString, Ctor)
    {
        TestChar c('f');
        TestString str(c);

        TestChar cs[12];
        for (int i = 0; i < 11; ++i)
        {
            cs[i] = TestChar('f');
        }
        TestString str2(cs);
    }

    TEST(String, Ctor)
    {
        String str1('f');
        String str2("fffffffffff");
        String str3(16, 'f');
        String str4(str2);
        EXPECT_TRUE(str2 == str4);
        EXPECT_FALSE(str4 == "ffffffffff");

        str4 = String::Format("format {0}", 1);
        EXPECT_TRUE(str4 == "format 1");
    }

    TEST(String, Empty)
    {
        String null;
        String empty("");
        EXPECT_TRUE(null.Empty() && empty.Empty());
    }

    TEST(String, Concat)
    {
        String str1("aabb");
        String str2;

        str1.Truncate(2);
        EXPECT_TRUE(str1 == "aa");

        str2.Truncate(0);
        EXPECT_TRUE(str2.Empty());

        str1 += "cc";
        EXPECT_TRUE(str1 == "aacc");

        str2.Append("");
        EXPECT_TRUE(str2.Empty());

        str2.Append("aa");
        EXPECT_TRUE(str2 == "aa");

        str1.Prepend(String("0x"));
        EXPECT_TRUE(str1 == "0xaacc");

        String str3;
        str3.Prepend("aa");
        EXPECT_TRUE(str3 == "aa");

        str1.Insert(2, "80");
        EXPECT_TRUE(str1 == "0x80aacc");

        String str4;
        str4.Insert(0, "aa");
        EXPECT_TRUE(str4 == "aa");

        EXPECT_TRUE(str4 / "bb" == "aa/bb");
    }

    TEST(String, Split)
    {
        String str1 = "0x80aacc";
        String str2 = str1.Slices(2, 4);
        String str3 = str1.Slices(-6, 4);

        EXPECT_TRUE(str2 == str3 && str2 == "80aa");

        String str4;
        EXPECT_TRUE(str4.Slices(0, 0).Empty());

        EXPECT_TRUE(str1.Remove(4, 1) == "0x80acc");
        EXPECT_TRUE(str1.Remove(6, 2) == "0x80ac");

        EXPECT_TRUE(str1.Remove("80") == "0xac");
        str1.Append("adac");
        EXPECT_TRUE(str1.Remove("ac") == "0xad");

        String str5 = "hello_world_hello_polaris";
        EXPECT_TRUE(str5.Replace("hello", "thanks") == "thanks_world_thanks_polaris");
        EXPECT_TRUE(str5.Replace("", "") == "thanks_world_thanks_polaris");
        EXPECT_TRUE(str5.Replace("thanks", "") == "_world__polaris");
    }

    TEST(String, Search)
    {
        String str1 = "ABC345efd";
        EXPECT_TRUE(str1.IndexOf("") == INDEX_NONE);
        EXPECT_TRUE(str1.IndexOf("AB") == 0);
        EXPECT_TRUE(str1.IndexOf("c3", CaseInsensitive) == 2);

        String str2 = "efd";
        EXPECT_TRUE(str1.IndexOf(str2) == 6);
        str2 = "eFd";
        EXPECT_TRUE(str1.IndexOf(str2, CaseInsensitive) == 6);

        EXPECT_TRUE(str1.IndexOf('4') == 4);

        EXPECT_TRUE(str1.IndexOfAny("") == INDEX_NONE);
        EXPECT_TRUE(str1.IndexOfAny("03") == 3);
        EXPECT_TRUE(str1.IndexOfAny("034") == 3);

        String str3 = "ABC345efdc301";
        EXPECT_TRUE(str3.LastIndexOf("") == INDEX_NONE);
        EXPECT_TRUE(str3.LastIndexOf("C3") == 2);
        EXPECT_TRUE(str3.LastIndexOf("C3", CaseInsensitive) == 9);
        EXPECT_TRUE(str3.LastIndexOfAny("9c", CaseInsensitive) == 9);

        EXPECT_TRUE(str3.LastIndexOfAny(str3.Contains("efd")));
        EXPECT_TRUE(str3.LastIndexOfAny(str3.Contains("eFd", CaseInsensitive)));

        String str4 = "abcdaef";
        EXPECT_TRUE(str4.Count('a') == 2);
        EXPECT_TRUE(str4.Count("ac") == 0);

        str4.Fill('f', 3);
        EXPECT_TRUE(str4 == "fff");
        str4.Fill('t');
        EXPECT_TRUE(str4 == "ttt");

        String str5 = str4.Repeated(3);
        EXPECT_TRUE(str5 == "ttttttttt");

        String str6 = "  hello. ";
        str6 = str6.Trimmed();
        EXPECT_TRUE(str6 == "hello.");

        EXPECT_TRUE(str6.IsLowerAscii());
        str6.ToUpperAscii();
        EXPECT_TRUE(str6 == "HELLO.");
        EXPECT_TRUE(str6.IsUpperAscii());
        str6.ToLowerAscii();
        EXPECT_TRUE(str6 == "hello.");

        String str7 = "80universe item snip";
        auto items = str7.Split("ni");
        EXPECT_TRUE(items.Size() == 3);
        items = str7.SplitAny("ni");
        EXPECT_TRUE(items.Size() == 6);
        items = str7.SplitAny("ni", SkipEmptyParts);
        EXPECT_TRUE(items.Size() == 4);
    }

    TEST(FileSystem, All)
    {
        for (const DirectoryEntry& entry : FileSystem::DirectoryIterator("C:\\Code\\PolarisEngine\\engine\\test"))
        {
            LOG_INFO("", "{0}", entry.GetPath());
        }
    }
}
