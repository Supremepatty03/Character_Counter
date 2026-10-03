#include "UnorderedMapCountingStrategy.h"

std::unordered_map<wchar_t, int>
UnorderedMapCountingStrategy::Count(
    const CharacterData& data
) const
{
    std::unordered_map<wchar_t, int> result;

    for (wchar_t symbol : data.GetSymbols())
    {
        result[symbol] = 0;
    }

    for (wchar_t symbol : data.GetText())
    {
        auto iterator = result.find(symbol);

        if (iterator != result.end())
        {
            ++iterator->second;
        }
    }

    return result;
}
