#include "Utf8Converter.h"

#ifdef _WIN32

#include <windows.h>

#include <stdexcept>

std::wstring Utf8Converter::ToWide(const std::string& text)
{
    if (text.empty())
    {
        return {};
    }

    const int wideLength = MultiByteToWideChar(
        CP_UTF8,
        MB_ERR_INVALID_CHARS,
        text.data(),
        static_cast<int>(text.size()),
        nullptr,
        0
    );

    if (wideLength <= 0)
    {
        throw std::runtime_error(
            "Failed to convert UTF-8 to Unicode."
        );
    }

    std::wstring result(wideLength, L'\0');

    if (MultiByteToWideChar(
            CP_UTF8,
            MB_ERR_INVALID_CHARS,
            text.data(),
            static_cast<int>(text.size()),
            result.data(),
            wideLength
        ) <= 0)
    {
        throw std::runtime_error(
            "Failed to convert UTF-8 to Unicode."
        );
    }

    return result;
}

std::string Utf8Converter::ToUtf8(const std::wstring& text)
{
    if (text.empty())
    {
        return {};
    }

    const int utf8Length = WideCharToMultiByte(
        CP_UTF8,
        WC_ERR_INVALID_CHARS,
        text.data(),
        static_cast<int>(text.size()),
        nullptr,
        0,
        nullptr,
        nullptr
    );

    if (utf8Length <= 0)
    {
        throw std::runtime_error(
            "Failed to convert Unicode to UTF-8."
        );
    }

    std::string result(utf8Length, '\0');

    if (WideCharToMultiByte(
            CP_UTF8,
            WC_ERR_INVALID_CHARS,
            text.data(),
            static_cast<int>(text.size()),
            result.data(),
            utf8Length,
            nullptr,
            nullptr
        ) <= 0)
    {
        throw std::runtime_error(
            "Failed to convert Unicode to UTF-8."
        );
    }

    return result;
}

#else

#include <codecvt>
#include <locale>

std::wstring Utf8Converter::ToWide(const std::string& text)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(text);
}

std::string Utf8Converter::ToUtf8(const std::wstring& text)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(text);
}

#endif
