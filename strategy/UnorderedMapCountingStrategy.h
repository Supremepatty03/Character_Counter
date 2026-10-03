#pragma once

#include "ICharacterCountingStrategy.h"

class UnorderedMapCountingStrategy final
    : public ICharacterCountingStrategy
{
public:
    std::unordered_map<wchar_t, int> Count(
        const CharacterData& data
    ) const override;
};
