#include <fluffycoin/p2p/BruteForce.h>

using namespace fluffycoin;
using namespace fluffycoin::p2p;

namespace
{

constexpr const unsigned int MAX_OFFENSES = 3;
constexpr const unsigned int LOCKOUT_SECONDS = 60;
constexpr const unsigned int CLEANUP_INTERVAL_SECONDS = 20;

}

bool BruteForce::checkAddress(
    const std::string &address)
{
    std::lock_guard<std::mutex> lock(mtx);

    cleanup(lock);
    auto iter = mapAddressToForgiveness.find(address);
    return iter == mapAddressToForgiveness.end();
}

void BruteForce::addOffense(
    const std::string &address)
{
    std::lock_guard<std::mutex> lock(mtx);

    // Already blocked?
    auto iterBlocked = mapAddressToForgiveness.find(address);
    if (iterBlocked != mapAddressToForgiveness.end())
        return;

    // Increment number of offenses
    auto iter = offenders.find(address);
    if (iter == offenders.end())
    {
        BruteForceOffender offender;
        offender.address = address;
        offender.offenses++;
        offender.lastOffense = std::chrono::steady_clock::now();
        offenders[address] = std::move(offender);
    }
    else
    {
        BruteForceOffender &offender = iter->second;
        offender.lastOffense = std::chrono::steady_clock::now();
        if (++offender.offenses >= MAX_OFFENSES)
        {
            mapAddressToForgiveness[address] = offender.lastOffense + std::chrono::seconds(LOCKOUT_SECONDS);
            offenders.erase(iter);
        }
    }
}

void BruteForce::cleanup(
    const std::lock_guard<std::mutex> &lock)
{
    (void)lock;

    auto now = std::chrono::steady_clock::now();
    if (nextCleanup < now)
        return;

    // Forgive previous transgressions
    nextCleanup = now + std::chrono::seconds(CLEANUP_INTERVAL_SECONDS);
    auto iter = mapAddressToForgiveness.begin();
    while (iter != mapAddressToForgiveness.end())
    {
        if (iter->second < now)
            iter = mapAddressToForgiveness.erase(iter);
        else
            ++iter;
    }

    // Reset pending offenses
    auto iterPending = offenders.begin();
    while (iterPending != offenders.end())
    {
        if (iterPending->second.lastOffense + std::chrono::seconds(LOCKOUT_SECONDS) < now)
            iterPending = offenders.erase(iterPending);
        else
            ++iterPending;
    }
}
