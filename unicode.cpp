#include "unicode.h"
#include <cassert>
#include <cstdint>

namespace
{
size_t round_up4(size_t x)
{
    return (x + 0x03ULL) & ~0x03ULL;
}
} // namespace

std::u8string utf16_to_utf8(const std::u16string& utf16_string)
{
    std::u8string utf8_bytes;
    utf8_bytes.reserve(round_up4(utf16_string.length()));
    for(size_t i = 0; i < utf16_string.length(); ++i) {
        char32_t code_point;
        char16_t unit = utf16_string[i];

        if(0xD800 <= unit && unit <= 0xDBFF) { // High surrogate
            if(i + 1 < utf16_string.length()) {
                char16_t next_unit = utf16_string[++i];
                if(0xDC00 <= next_unit && next_unit <= 0xDFFF) { // Low surrogate
                    code_point = 0x10000 + ((unit - 0xD800) << 10) + (next_unit - 0xDC00);
                } else {
                    // Handle malformed UTF-16 (e.g., lone high surrogate)
                    // For this example, we'll just treat it as a single unit
                    code_point = unit;
                }
            } else {
                // Handle malformed UTF-16 (e.g., lone high surrogate at end)
                code_point = unit;
            }
        } else {
            code_point = unit;
        }

        // Encode code_point to UTF-8
        if(code_point <= 0x7F) {
            utf8_bytes.push_back(static_cast<char8_t>(code_point));
        } else if(code_point <= 0x7FF) {
            utf8_bytes.push_back(static_cast<char8_t>(0xC0 | (code_point >> 6)));
            utf8_bytes.push_back(static_cast<char8_t>(0x80 | (code_point & 0x3F)));
        } else if(code_point <= 0xFFFF) {
            utf8_bytes.push_back(static_cast<char8_t>(0xE0 | (code_point >> 12)));
            utf8_bytes.push_back(static_cast<char8_t>(0x80 | ((code_point >> 6) & 0x3F)));
            utf8_bytes.push_back(static_cast<char8_t>(0x80 | (code_point & 0x3F)));
        } else if(code_point <= 0x10FFFF) {
            utf8_bytes.push_back(static_cast<char8_t>(0xF0 | (code_point >> 18)));
            utf8_bytes.push_back(static_cast<char8_t>(0x80 | ((code_point >> 12) & 0x3F)));
            utf8_bytes.push_back(static_cast<char8_t>(0x80 | ((code_point >> 6) & 0x3F)));
            utf8_bytes.push_back(static_cast<char8_t>(0x80 | (code_point & 0x3F)));
        }
    }
    return utf8_bytes;
}

namespace
{
// Function to convert a Unicode codepoint to UTF-16 code units
size_t codepoint_to_utf16(char16_t utf16_units[4], char32_t codepoint)
{
    size_t length = 0;
    if(codepoint <= 0xFFFF) {
        utf16_units[length++] = static_cast<char16_t>(codepoint);
    } else {
        char32_t temp = codepoint - 0x10000;
        char16_t high_surrogate = static_cast<char16_t>(0xD800 + (temp >> 10));
        char16_t low_surrogate = 0xDC00 + (temp & 0x3FF);
        utf16_units[length++] = high_surrogate;
        utf16_units[length++] = low_surrogate;
    }
    return length;
}

// Simplified function to decode a single UTF-8 sequence to a codepoint
// (This is a simplified example and does not handle all edge cases or error checking)
char32_t decode_to_codepoint(const std::u8string& utf8_string, size_t& index)
{
    assert(0 < utf8_string.length());
    char32_t byte1 = utf8_string[index++];
    if((byte1 & 0x80) == 0) { // 1-byte sequence
        return byte1;
    } else if((byte1 & 0xE0) == 0xC0) { // 2-byte sequence
        if((utf8_string.length() - index) < 1) {
            return 0;
        }
        char32_t byte2 = utf8_string[index++];
        return ((byte1 & 0x1F) << 6) | (byte2 & 0x3F);
    } else if((byte1 & 0xF0) == 0xE0) { // 3-byte sequence
        if((utf8_string.length() - index) < 2) {
            return 0;
        }
        char32_t byte2 = utf8_string[index++];
        char32_t byte3 = utf8_string[index++];
        return ((byte1 & 0x0F) << 12) | ((byte2 & 0x3F) << 6) | (byte3 & 0x3F);
    } else if((byte1 & 0xF8) == 0xF0) { // 4-byte sequence
        if((utf8_string.length() - index) < 3) {
            return 0;
        }
        char32_t byte2 = utf8_string[index++];
        char32_t byte3 = utf8_string[index++];
        char32_t byte4 = utf8_string[index++];
        return ((byte1 & 0x07) << 18) | ((byte2 & 0x3F) << 12) | ((byte3 & 0x3F) << 6) | (byte4 & 0x3F);
    }
    return 0; // Error or invalid sequence
}
} // namespace

std::u16string utf8_to_utf16(const std::u8string& utf8_string)
{
    std::u16string utf16_string;
    utf16_string.reserve(round_up4(utf8_string.length()));
    size_t index = 0;
    char16_t utf16_units[4];
    while(index < utf8_string.length()) {
        char32_t codepoint = decode_to_codepoint(utf8_string, index);
        if(0 == codepoint) {
            break;
        }
        size_t length = codepoint_to_utf16(utf16_units, codepoint);
        for(size_t i = 0; i < length; ++i) {
            utf16_string.push_back(utf16_units[i]);
        }
    }
    return utf16_string;
}
