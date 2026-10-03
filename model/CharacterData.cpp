#include "CharacterData.h"

CharacterData::CharacterData(
    const std::wstring& text,
    const std::vector<wchar_t>& symbols
)
    : text_(text),
      symbols_(symbols)
{
}

const std::wstring& CharacterData::GetText() const
{
    return text_;
}

const std::vector<wchar_t>& CharacterData::GetSymbols() const
{
    return symbols_;
}

void CharacterData::SetText(const std::wstring& text)
{
    text_ = text;
}

void CharacterData::SetSymbols(
    const std::vector<wchar_t>& symbols
)
{
    symbols_ = symbols;
}
