#pragma once

#include "../strategy/ICharacterCountingStrategy.h"

#include <memory>

class CountingStrategyFactory
{
public:
    static std::unique_ptr<ICharacterCountingStrategy> Create();
};
