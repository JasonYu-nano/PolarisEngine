#include "string_algorithm.hpp"
#include "ustring_matcher.hpp"


namespace Engine::Private
{

    strsize UStringMatcher::IndexIn(UStringView& str, strsize from) const
    {
        from = from < 0 ? 0 : from;

        if (CS == ECaseSensitivity::Sensitive)
        {
            return _BMFind<UChar>(str.Data(), str.Length(), from, PatternView.Data(), PatternView.Length(), SkipTable);
        }
        else
        {
            return UBMFindInsensitive(K_UCHAR_TO_UTF16(str.Data()), str.Length(), from, K_UCHAR_TO_UTF16(PatternView.Data()), PatternView.Length(), K_UCHAR_TO_UTF16(SkipTable));
        }
    }

    void UStringMatcher::UpdateSkipTable()
    {
        if (CS == ECaseSensitivity::Sensitive)
        {
            _BMInitSkipTable<UChar>(PatternView.Data(), PatternView.Length(), SkipTable);
        }
        else
        {
            UBMInitSkipTableInsensitive(K_UCHAR_TO_UTF16(PatternView.Data()), PatternView.Length(), UCHAR_TO_UTF16(SkipTable));
        }
    }
}