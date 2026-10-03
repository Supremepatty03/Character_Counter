#pragma once

#include "../model/CharacterData.h"

#include <unordered_map>

class ICharacterCountingStrategy
{
public:
    virtual ~ICharacterCountingStrategy() = default;

    virtual std::unordered_map<wchar_t, int> Count(
        const CharacterData& data
    ) const = 0;
};
