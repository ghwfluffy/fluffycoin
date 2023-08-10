#pragma once

#include <fluffycoin/async/TreasureTrove.h>

#include <memory>
#include <functional>

namespace fluffycoin::async
{

/**
 * Wrapper for a Treasure class that will use RAII to automatically manage
 * the memory that will be used for a serial sequence of async operations
 */
template<typename TreasureType>
class BuriedTreasure
{
    public:
        template<typename TT=TreasureType, typename... Args>
        BuriedTreasure(
            TreasureTrove &trove,
            Args&... args)
                : ptrove(&trove)
        {
            std::unique_ptr<TT> uptreasure = std::make_unique<TT>(std::forward<Args>(args)...);
            ptreasure = uptreasure.get();
            trove.add(std::move(uptreasure));
        }
        BuriedTreasure(BuriedTreasure<TreasureType> &&rhs)
        {
            operator=(std::move(rhs));
        }
        BuriedTreasure(const BuriedTreasure<TreasureType> &) = delete;
        BuriedTreasure &operator=(BuriedTreasure<TreasureType> &&rhs)
        {
            if (this != &rhs)
            {
                cleanup();
                ptrove = rhs.ptrove;
                ptreasure = rhs.ptreasure;
            }
            return (*this);
        }
        BuriedTreasure &operator=(const BuriedTreasure<TreasureType> &) = delete;
        ~BuriedTreasure()
        {
            cleanup();
        }

        TreasureType &operator()() const
        {
            return *ptreasure;
        }

    private:
        void cleanup()
        {
            if (ptrove && ptreasure)
                ptrove->cleanup(*ptreasure);
            ptreasure = nullptr;
        }

        TreasureTrove *ptrove = nullptr;
        TreasureType *ptreasure = nullptr;
};

}
