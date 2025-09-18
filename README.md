# uconv

`uconv` is a C++ library for converting strings between UTF-8 and UTF-16 encoding. It is designed to be simple to use and integrate into your projects.

## Features
- Make use of `char8_t` of C++20.
- Convert between `std::u8string` (UTF-8) and `std::u16string` (UTF-16).

## Usage

To use `uconv`, put `uconv.h` and `uconv.cpp` into your project.

```cpp
#include <iostream>
#include "uconv.h"

int main(void)
{
    using namespace uconv;
    std::u8string utf8_string = u8"こんにちは、世界！";
    std::u16string utf16_string = utf8_to_utf16(utf8_string);
    std::u8string utf8_roundtrip = utf16_to_utf8(utf16_string);
    std::cout << reinterpret_cast<const char*>(utf8_roundtrip.c_str()) << std::endl;
    return 0;
}
```

## License

This project is licensed under the MIT License or the Public Domain - see the [LICENSE](LICENSE) file for details.
