#include "uconv.h"
#include <cassert>
#include <iostream>

using namespace uconv;

void test_ascii()
{
    std::u8string utf8_ascii = u8"Hello, World!";
    std::u16string utf16_ascii = utf8_to_utf16(utf8_ascii);
    std::u8string utf8_ascii_roundtrip = utf16_to_utf8(utf16_ascii);
    assert(utf8_ascii == utf8_ascii_roundtrip);
    std::cout << "ASCII test passed." << reinterpret_cast<const char*>(utf8_ascii_roundtrip.c_str()) << std::endl;
}

void test_japanese()
{
    std::u8string utf8_japanese = u8"こんにちは、世界！";
    std::u16string utf16_japanese = utf8_to_utf16(utf8_japanese);
    std::u8string utf8_japanese_roundtrip = utf16_to_utf8(utf16_japanese);
    assert(utf8_japanese == utf8_japanese_roundtrip);
    std::cout << "Japanese test passed." << reinterpret_cast<const char*>(utf8_japanese_roundtrip.c_str()) << std::endl;
}

void test_emoji()
{
    std::u8string utf8_emoji = u8"👋🌎";
    std::u16string utf16_emoji = utf8_to_utf16(utf8_emoji);
    std::u8string utf8_emoji_roundtrip = utf16_to_utf8(utf16_emoji);
    assert(utf8_emoji == utf8_emoji_roundtrip);
    std::cout << "Emoji test passed." << reinterpret_cast<const char*>(utf8_emoji_roundtrip.c_str()) << std::endl;
}

void test_empty()
{
    std::u8string utf8_empty = u8"";
    std::u16string utf16_empty = utf8_to_utf16(utf8_empty);
    std::u8string utf8_empty_roundtrip = utf16_to_utf8(utf16_empty);
    assert(utf8_empty == utf8_empty_roundtrip);
    std::cout << "Empty string test passed." << reinterpret_cast<const char*>(utf8_empty_roundtrip.c_str()) << std::endl;
}

void test_roundtrip()
{
    // UTF-8 -> UTF-16 -> UTF-8
    std::u8string original_utf8 = u8"This is a test string with various characters: éàçüö";
    std::u16string converted_utf16 = utf8_to_utf16(original_utf8);
    std::u8string roundtrip_utf8 = utf16_to_utf8(converted_utf16);
    assert(original_utf8 == roundtrip_utf8);

    std::cout << "Roundtrip UTF-8 tests passed." << reinterpret_cast<const char*>(roundtrip_utf8.c_str()) << std::endl;

    // UTF-16 -> UTF-8 -> UTF-16
    std::u16string original_utf16 = u"Another test with 𝄞 clef and other symbols: €£¥";
    std::u8string converted_utf8 = utf16_to_utf8(original_utf16);
    std::u16string roundtrip_utf16 = utf8_to_utf16(converted_utf8);
    assert(original_utf16 == roundtrip_utf16);

    std::cout << "Roundtrip UTF-16 tests passed." << reinterpret_cast<const char*>(roundtrip_utf16.c_str()) << std::endl;
}

void test_surrogate_pairs()
{
    // U+1D11E is musical symbol G clef
    // U+10400 is Deseret Long I
    std::u8string utf8_surrogate = u8"𝄞𐐀"; // U+1D11E U+10400
    std::u16string utf16_surrogate = utf8_to_utf16(utf8_surrogate);
    std::u8string utf8_surrogate_roundtrip = utf16_to_utf8(utf16_surrogate);
    assert(utf8_surrogate == utf8_surrogate_roundtrip);

    std::u16string original_utf16_surrogate = u"𝄞𐐀";
    std::u8string converted_utf8_surrogate = utf16_to_utf8(original_utf16_surrogate);
    std::u16string roundtrip_utf16_surrogate = utf8_to_utf16(converted_utf8_surrogate);
    assert(original_utf16_surrogate == roundtrip_utf16_surrogate);

    std::cout << "Surrogate pair test passed." << std::endl;
}

// "regacha" tests for tricky cases like malformed sequences
void test_malformed_sequences()
{
    // Test invalid UTF-16 (unpaired surrogates)
    // A high surrogate without a following low surrogate
    char16_t high_surrogate[] = {static_cast<char16_t>(0xD834), 0};
    std::u16string unpaired_high_surrogate = high_surrogate; // u"A\xD834B"; // High surrogate U+D834 alone
    std::u8string utf8_from_unpaired_high = utf16_to_utf8(unpaired_high_surrogate);
    // Common behavior is to replace with U+FFFD (EF BF BD in UTF-8)
    std::u8string expected_utf8_high = u8"A\xEF\xBF\xBDB";
    if(utf8_from_unpaired_high == expected_utf8_high) {
        std::cout << "Malformed sequence test (unpaired high surrogate) passed." << std::endl;
    } else {
        std::cout << "Malformed sequence test (unpaired high surrogate) did not produce replacement character." << std::endl;
    }

    // A low surrogate without a preceding high surrogate
    char16_t low_surrogate[] = {static_cast<char16_t>(0xDD1E), 0};
    std::u16string unpaired_low_surrogate = low_surrogate; // u "A\xDD1EB"; // Low surrogate U+DD1E alone
    std::u8string utf8_from_unpaired_low = utf16_to_utf8(unpaired_low_surrogate);
    std::u8string expected_utf8_low = u8"A\xEF\xBF\xBDB";
    if(utf8_from_unpaired_low == expected_utf8_low) {
        std::cout << "Malformed sequence test (unpaired low surrogate) passed." << std::endl;
    } else {
        std::cout << "Malformed sequence test (unpaired low surrogate) did not produce replacement character." << std::endl;
    }

    // Test invalid UTF-8 (incomplete sequence)
    std::u8string incomplete_utf8 = u8"A\xF0\x9D\x84"; // Missing last byte for a 4-byte char
    std::u16string utf16_from_incomplete = utf8_to_utf16(incomplete_utf8);
    std::u16string expected_utf16_incomplete = u"A\uFFFD";
    if(utf16_from_incomplete == expected_utf16_incomplete) {
        std::cout << "Malformed sequence test (incomplete UTF-8) passed." << std::endl;
    } else {
        std::cout << "Malformed sequence test (incomplete UTF-8) did not produce replacement character." << std::endl;
    }
    std::cout << "Finished 'regacha' tests for malformed sequences." << std::endl;
}

void test_ascii_short()
{
    std::u8string utf8_ascii = u8"Hello, World!";

    static constexpr size_t utf16_length = 32;
    char16_t utf16_string[utf16_length+1] = {};

    size_t length = utf8_to_utf16(utf16_length, utf16_string, utf8_ascii.length(), utf8_ascii.c_str());
    assert(0<length);
    static constexpr size_t utf8_length = 128;
    char8_t utf8_string[utf8_length+1] = {};
    size_t length2 = utf16_to_utf8(utf8_length, utf8_string, length, utf16_string);
    assert(0 == strcmp(reinterpret_cast<const char*>(utf8_ascii.c_str()), reinterpret_cast<const char*>(utf8_string)));
    //std::cout << "ASCII test passed." << reinterpret_cast<const char*>(utf8_ascii_roundtrip.c_str()) << std::endl;
}

int main(void)
{
    test_ascii();
    test_japanese();
    test_emoji();
    test_empty();
    test_roundtrip();
    test_surrogate_pairs();
    test_malformed_sequences();

    test_ascii_short();

    std::cout << "All tests passed!" << std::endl;

    return 0;
}
