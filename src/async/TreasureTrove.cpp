#include <fluffycoin/async/TreasureTrove.h>

using namespace fluffycoin;
using namespace fluffycoin::async;

void TreasureTrove::add(std::unique_ptr<Treasure> mem)
{
    std::lock_guard<std::mutex> lock(mtx);
    memory.push_back(std::move(mem));
}

void TreasureTrove::cleanup(const Treasure &item)
{
    std::lock_guard<std::mutex> lock(mtx);
    auto iter = memory.begin();
    while (iter != memory.end())
    {
        if (iter->get() == &item)
        {
            memory.erase(iter);
            break;
        }
    }
}
