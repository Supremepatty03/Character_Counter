#pragma once

#include <string>
#include <vector>

class CharacterData
{
public:
    CharacterData() = default;

    CharacterData(
        const std::wstring& text,
        const std::vector<wchar_t>& symbols
    );

    const std::wstring& GetText() const;
    const std::vector<wchar_t>& GetSymbols() const;

    void SetText(const std::wstring& text);
    void SetSymbols(const std::vector<wchar_t>& symbols);

private:
    std::wstring text_;
    std::vector<wchar_t> symbols_;
};
