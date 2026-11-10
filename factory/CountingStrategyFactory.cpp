#include "CountingStrategyFactory.h"

#include "../strategy/UnorderedMapCountingStrategy.h"

using namespace std;

unique_ptr<ICharacterCountingStrategy>
CountingStrategyFactory::Create()
{
    return make_unique<UnorderedMapCountingStrategy>();
}
