#ifndef INC_UCONV_H_
#define INC_UCONV_H_
#include <cstdint>
#include <string>

namespace uconv
{
/**
 * @brief Converts a UTF-16 encoded string to a UTF-8 encoded string.
 *
 * This function iterates over each UTF-16 code unit in the input string and
 * produces a UTF-8 encoded output. It properly handles surrogate pairs,
 * converting them to valid Unicode code points in the range U+10000–U+10FFFF.
 *
 * Malformed UTF-16 sequences (such as an unmatched high surrogate or a lone
 * low surrogate) are handled gracefully: they are treated as individual code
 * units and directly encoded into UTF-8.
 *
 * @param utf16_string The input UTF-16 encoded string to be converted.
 * @return A std::u8string containing the UTF-8 encoded result.
 * @note The function reserves memory in multiples of 4 bytes for efficiency,
 *       but the actual output length may be smaller than the reserved capacity.
 * @warning Invalid or malformed UTF-16 sequences are not rejected. they are
 *          encoded as-is. Applications requiring strict validation of UTF-16
 *          input should handle validation before calling this function.
 */
std::u8string utf16_to_utf8(const std::u16string& utf16_string);

/**
 * @brief Converts a UTF-8 encoded string to a UTF-16 encoded string.
 *
 * This function decodes each UTF-8 sequence in the input string into a Unicode
 * code point, and then encodes that code point into one or two UTF-16 code units.
 * Surrogate pairs are generated when the code point is outside the Basic Multilingual
 * Plane (U+10000 – U+10FFFF).
 *
 * The function relies on helper functions:
 * - decode_to_codepoint(): decodes a single UTF-8 sequence into a Unicode code point.
 * - codepoint_to_utf16(): converts a Unicode code point into UTF-16 code units.
 *
 * @param utf8_string The input UTF-8 encoded string to be converted.
 * @return A std::u16string containing the UTF-16 encoded result.
 * @note The function reserves memory in multiples of 4 code units for efficiency,
 *       but the actual output length may be smaller.
 * @warning Malformed UTF-8 sequences result in a `0` code point and cause the
 *          conversion process to stop early. Applications requiring strict error
 *          handling should validate UTF-8 input before calling this function.
 */
std::u16string utf8_to_utf16(const std::u8string& utf8_string);

/**
 * @brief Converts a UTF-16 encoded string to a UTF-8 encoded string (into a user-provided buffer).
 *
 * This function decodes a UTF-16 sequence into Unicode code points and encodes them
 * into UTF-8, storing the result in the provided output buffer. It correctly handles
 * surrogate pairs, converting them into code points in the supplementary planes
 * (U+10000 – U+10FFFF). Malformed UTF-16 sequences (such as a lone high surrogate or
 * low surrogate) are treated as individual code units and directly encoded.
 *
 * @param utf8_length The size of the output buffer, in bytes. Must be at least 128.
 * @param utf8_string Pointer to the output buffer where the UTF-8 encoded data
 *                    will be written. Must not be null.
 * @param utf16_length The number of UTF-16 code units in the input string. Must be at least 1.
 * @param utf16_string Pointer to the UTF-16 encoded input string. Must not be null.
 * @return The number of bytes written to @p utf8_string on success.
 *         Returns 0 if the output buffer is too small to hold the full conversion.
 * @note The function performs boundary checks before writing multi-byte UTF-8 sequences.
 *       If insufficient space remains in the buffer, the function stops and returns 0.
 * @warning The function does not append a null terminator to the output buffer.
 *          The caller must handle null-termination if required.
 */
size_t utf16_to_utf8(size_t utf8_length, char8_t* utf8_string, size_t utf16_length, const char16_t* utf16_string);

/**
 * @brief Converts a UTF-8 encoded string to a UTF-16 encoded string (into a user-provided buffer).
 *
 * This function decodes UTF-8 byte sequences into Unicode code points and encodes
 * them into UTF-16 code units, writing the result into the provided output buffer.
 * Surrogate pairs are generated for code points outside the Basic Multilingual Plane
 * (U+10000 – U+10FFFF). Malformed UTF-8 sequences result in a `0` code point and
 * terminate the conversion early.
 *
 * @param utf16_length The size of the output buffer, in code units. Must be at least 64.
 * @param utf16_string Pointer to the output buffer where UTF-16 code units will be written.
 *                     Must not be null.
 * @param utf8_length The number of bytes in the input UTF-8 string. Must be at least 1.
 * @param utf8_string Pointer to the input UTF-8 encoded string. Must not be null.
 * @return The number of UTF-16 code units written to @p utf16_string on success.
 *         Returns the count accumulated before encountering an invalid sequence.
 * @note The function does not append a null terminator to the output buffer. The caller
 *       must handle null-termination if required.
 * @warning If a malformed UTF-8 sequence is detected, conversion stops immediately.
 *          Applications requiring strict validation should sanitize input beforehand.
 */
size_t utf8_to_utf16(size_t utf16_length, char16_t* utf16_string, size_t utf8_length, const char8_t* utf8_string);
} // namespace uconv
#endif // INC_UCONV_H_
