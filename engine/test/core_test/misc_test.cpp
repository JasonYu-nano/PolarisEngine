#include "gtest/gtest.h"
#include "core_minimal_public.hpp"
#include "foundation/char_utils.hpp"
#include "math/city_hash.hpp"
#include "foundation/fixed_string.hpp"
#include "foundation/ustring.hpp"

namespace Engine
{
    TEST(CharTest, Base)
    {
        ansi c = 'C';
        c = CharUtils::ToLower(c);
        EXPECT_TRUE(c == 'c');

        c = CharUtils::ToUpper(c);
        EXPECT_TRUE(c == 'C');

        EXPECT_TRUE(CharUtils::Compare(u"A", "A") == 0);

        UString str = "a";
        EXPECT_TRUE(CharUtils::Compare(str.Data(), "A") == 1);

        UChar uch = 0x6C49;
        CharUtils::Compare(&uch, U"你好");
        //uch.ToLatin1();
    }

    TEST(FixedString, Base)
    {
        FixedString name(_T("Hello_World"));
        EXPECT_TRUE(name.ToString() == _T("Hello_World"));

        FixedString name2(_T("Hello_World_12"));
        EXPECT_TRUE(name2.ToString() == _T("Hello_World_12") && name2.GetNumber() == 12);

        FixedString name3(_T("Hello_World_012"));
        EXPECT_TRUE(name3.GetNumber() == 0);

        FixedString name4(_T("hello_World_12"));
        EXPECT_TRUE(name2 == name4);
        EXPECT_TRUE(name4.ToString() == _T("Hello_World_12"));
    }

    TEST(UChar, Base)
    {
        UChar ch('A');
        ch.ToLower();
        EXPECT_TRUE(ch == 'a');
    }

    TEST(StringView, Ctor)
    {
        StringView view = "abc";
    }

    TEST(UString, Ctor)
    {
        UString str = UString('a');
        EXPECT_TRUE(str[0] == 'a');

        str = UString::FromLatin1("1234\0");
        EXPECT_TRUE(str[0] == '1' && str[1] == '2' && str[2] == '3' && str[3] == '4');

        str = UString::FromUtf8("你好");
        UString str2 = UString::FromStdString("你好");
        EXPECT_TRUE(str == str2);

        auto u8 = str.ToUtf8();
        EXPECT_TRUE(UString::FromUtf8(u8.Data()) == str);

        EXPECT_TRUE(str.ToStdString() == u8.Data());
    }

    TEST(UString, Concat)
    {
        UString str = "abcd";
        UString str2 = "1234";
        str.Append(str2);
        EXPECT_TRUE(str == "abcd1234");

        str.Prepend(str2);
        EXPECT_TRUE(str == "1234abcd1234");
    }

    TEST(UString, Compare)
    {
        UString str("ABCd");
        UString str2("abcd");

        EXPECT_FALSE(str.StartsWith(str2));
        EXPECT_TRUE(str.StartsWith(str2, ECaseSensitivity::Insensitive));

        EXPECT_TRUE(str.StartsWith(UString("ABCd")));
        EXPECT_FALSE(str.StartsWith(UString("ABCde")));

        EXPECT_FALSE(str.StartsWith(UChar('a')));
        EXPECT_TRUE(str.StartsWith(UChar('a'), ECaseSensitivity::Insensitive));

        EXPECT_FALSE(str.StartsWith("abc"));
        EXPECT_TRUE(str.StartsWith("abc", ECaseSensitivity::Insensitive));

        EXPECT_FALSE(str.EndsWith(str2));
        EXPECT_TRUE(str.EndsWith(str2, ECaseSensitivity::Insensitive));

        EXPECT_TRUE(str.EndsWith(UString("Cd")));
        EXPECT_FALSE(str.EndsWith(UString("ABCde")));

        EXPECT_FALSE(str.EndsWith(UChar('D')));
        EXPECT_TRUE(str.EndsWith(UChar('d'), ECaseSensitivity::Insensitive));

        EXPECT_FALSE(str.EndsWith("cd"));
        EXPECT_TRUE(str.EndsWith("CD", ECaseSensitivity::Insensitive));

        EXPECT_TRUE(str.Compare("ABCd") == 0);
        EXPECT_TRUE(str.Compare("abcd", ECaseSensitivity::Insensitive) == 0);
        EXPECT_TRUE(str.Compare("abc") == -1);
        EXPECT_TRUE(str.Compare("ABCD") == 1);
    }

    TEST(UString, Split)
    {
        UString str("1234.567");
        EXPECT_TRUE(*--str.Chopped(4).end() == '4');

        str.Chop(4);
        EXPECT_TRUE(*--str.end() == '4');
    }

    TEST(UString, Case)
    {
        UString str("ABC");
        EXPECT_TRUE(str.IsUpper());

        str.Append('d');
        EXPECT_FALSE(str.IsUpper());

        str.ToUpper();
        EXPECT_TRUE(str.IsUpper());

        str.ToLower();
        EXPECT_TRUE(str.IsLower());
    }

    TEST(UString, Find)
    {
        UString str("ABC345pc");
        UString sub("C345");
        EXPECT_TRUE(str.Contains(sub));
        EXPECT_TRUE(str.IndexOf(sub) == 2);

        sub.Append('P');
        EXPECT_FALSE(str.Contains(sub));

        EXPECT_TRUE(str.Contains(sub, ECaseSensitivity::Insensitive));
        EXPECT_TRUE(str.IndexOf(sub, ECaseSensitivity::Insensitive) == 2);
        EXPECT_TRUE(str.IndexOf("5P", ECaseSensitivity::Insensitive) == 5);

        PL_INFO("", "hello {0}", str);
        fmt::format(u"aaaa");

        str = UString::Formats("name is {0}", str);
        PL_INFO("", "hello {0}", str);
    }

    TEST(UString, Modify)
    {
        UString str("123456");
        str.Fill('a', -1);
        EXPECT_TRUE(str == "aaaaaa");
        str.Fill('b', 3);
        EXPECT_TRUE(str == "bbb");
        str.Fill('c', 0);
        EXPECT_TRUE(str.IsEmpty());

        str = "test";
        UString rep = str.Repeated(2);
        EXPECT_TRUE(rep == "testtest");

        rep = str.Repeated(1);
        EXPECT_TRUE(rep == str);

        rep = str.Repeated(0);
        EXPECT_TRUE(rep.IsNull());

        str = "test";
        UString removeTest = "123test";
        removeTest.Remove(str);
        EXPECT_TRUE(removeTest == "123");

        removeTest = "test123test";
        removeTest.Remove(str);
        EXPECT_TRUE(removeTest == "123");

        removeTest = "test123test";
        removeTest.Remove("test");
        EXPECT_TRUE(removeTest == "123");

        removeTest.Remove('2');
        EXPECT_TRUE(removeTest == "13");

        removeTest = "testtest";
        removeTest.Remove(str);
        EXPECT_TRUE(removeTest.IsEmpty());

        removeTest = "test";
        removeTest.Remove(1, 1);
        EXPECT_TRUE(removeTest == "tst");

        removeTest = "  lots\t of\nwhitespace\r\n ";
        removeTest = removeTest.Trimmed();
        EXPECT_TRUE(removeTest == "lots\t of\nwhitespace");

        std::string_view a = "111";
    }
}
