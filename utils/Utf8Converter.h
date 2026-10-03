#pragma once

#include <string>

class Utf8Converter
{
public:
    static std::wstring ToWide(const std::string& text);
    static std::string ToUtf8(const std::wstring& text);
};
