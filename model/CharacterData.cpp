#include "CharacterData.h"
using namespace std;

CharacterData::CharacterData(
    const wstring& text,
    const vector<wchar_t>& symbols
)
    : text_(text),
      symbols_(symbols)
{
}

const wstring& CharacterData::GetText() const
{
    return text_;
}

const vector<wchar_t>& CharacterData::GetSymbols() const
{
    return symbols_;
}

void CharacterData::SetText(const wstring& text)
{
    text_ = text;
}

void CharacterData::SetSymbols(
    const vector<wchar_t>& symbols
)
{
    symbols_ = symbols;
}
