#pragma once

#include <fluffycoin/utils/SafeData.h>

#include <map>
#include <mutex>
#include <chrono>
#include <string>

namespace fluffycoin::validator
{

struct PeerSession
{
    unsigned int protocolVersion = 0;
    std::string address;
    std::string sessionId;
    SafeData hmacKey;
    std::chrono::time_point<std::chrono::steady_clock> lastUse;
};

/**
 * Holds previously established handshakes with peers
 */
class PeerSessions
{
    public:
        PeerSessions() = default;
        PeerSessions(PeerSessions &&) = default;
        PeerSessions(const PeerSessions &) = default;
        PeerSessions &operator=(PeerSessions &&) = default;
        PeerSessions &operator=(const PeerSessions &) = default;
        ~PeerSessions() = default;

        void addSession(
            unsigned int protocolVersion,
            std::string peerAddress,
            std::string sessionId,
            SafeData hmacKey);

        PeerSession getSession(
            const std::string &sessionId);

    private:
        void cleanup(
            const std::lock_guard<std::mutex> &lock);

        std::mutex mtx;
        std::map<std::string, PeerSession> sessions;
        std::chrono::time_point<std::chrono::steady_clock> nextCleanup;
};

}
