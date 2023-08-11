#pragma once

#include <map>
#include <list>
#include <mutex>

#include <string>
#include <chrono>

namespace fluffycoin::p2p
{

/**
 * Stop peers from being obnoxious
 */
class BruteForce
{
    public:
        BruteForce() = default;
        BruteForce(BruteForce &&) = default;
        BruteForce(const BruteForce &) = default;
        BruteForce &operator=(BruteForce &&) = default;
        BruteForce &operator=(const BruteForce &) = default;
        ~BruteForce() = default;

        bool checkAddress(
            const std::string &authAddress);

        void addOffense(
            const std::string &address);

    private:
        void cleanup(
            const std::lock_guard<std::mutex> &lock);

        struct BruteForceOffender
        {
            std::string address;
            unsigned int offenses = 0;
            std::chrono::time_point<std::chrono::steady_clock> lastOffense;
        };

        std::mutex mtx;
        std::map<std::string, BruteForceOffender> offenders;
        std::map<std::string, std::chrono::time_point<std::chrono::steady_clock>> mapAddressToForgiveness;
        std::chrono::time_point<std::chrono::steady_clock> nextCleanup;
};

}
