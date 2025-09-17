#ifndef INC_UNICODE_H_
#define INC_UNICODE_H_
#include <string>

std::u8string utf16_to_utf8(const std::u16string& utf16_string);
std::u16string utf8_to_utf16(const std::u8string& utf8_string);
#endif // INC_UNICODE_H_

