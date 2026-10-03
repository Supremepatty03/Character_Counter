#include "CountingStrategyFactory.h"

#include "../strategy/UnorderedMapCountingStrategy.h"

std::unique_ptr<ICharacterCountingStrategy>
CountingStrategyFactory::Create()
{
    return std::make_unique<UnorderedMapCountingStrategy>();
}
