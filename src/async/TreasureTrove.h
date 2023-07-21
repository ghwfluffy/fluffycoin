#pragma once

#include <list>
#include <mutex>
#include <memory>

namespace fluffycoin::async
{

/**
 * Interface for freeable asynchronous state memory
 */
class Treasure
{
    public:
        virtual ~Treasure() = default;

    protected:
        Treasure() = default;
        Treasure(Treasure &&) = default;
        Treasure(const Treasure &) = default;
        Treasure &operator=(Treasure &&) = default;
        Treasure &operator=(const Treasure &) = default;
};

/**
 * Holds memory associated with an ongoing task. This class must be used appropriately
 * to assure all memory assigned to it is freed or it will silently leak until destruction.
 * The name is a play on garbage collector since it is a place to store useful memory.
 */
class TreasureTrove
{
    public:
        TreasureTrove() = default;
        TreasureTrove(TreasureTrove &&) = delete;
        TreasureTrove(const TreasureTrove &) = delete;
        TreasureTrove &operator=(TreasureTrove &&) = delete;
        TreasureTrove &operator=(const TreasureTrove &) = delete;
        ~TreasureTrove() = default;

        void add(std::unique_ptr<Treasure> mem);
        void cleanup(const Treasure &);

    private:
        std::mutex mtx;
        std::list<std::unique_ptr<Treasure>> memory;
};

}
