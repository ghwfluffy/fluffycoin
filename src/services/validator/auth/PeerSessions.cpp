#include <fluffycoin/validator/PeerSessions.h>

using namespace fluffycoin;
using namespace fluffycoin::validator;

namespace
{

constexpr const unsigned int SESSION_TIMEOUT_SECONDS = 300;

}

/**
 * Holds previously established handshakes with peers
 */
void PeerSessions::addSession(
    unsigned int protocolVersion,
    std::string peerAddress,
    std::string sessionId,
    SafeData hmacKey)
{
    std::lock_guard<std::mutex> lock(mtx);
    cleanup(lock);

    PeerSession &session = sessions[sessionId];
    session.protocolVersion = protocolVersion;
    session.address = std::move(peerAddress);
    session.sessionId = std::move(sessionId);
    session.hmacKey = std::move(hmacKey);
    session.lastUse = std::chrono::steady_clock::now();
}

PeerSession PeerSessions::getSession(
    const std::string &sessionId)
{
    std::lock_guard<std::mutex> lock(mtx);

    auto iter = sessions.find(sessionId);
    if (iter != sessions.end())
    {
        PeerSession &session = iter->second;
        session.lastUse = std::chrono::steady_clock::now();
        return session;
    }
    return PeerSession();
}

void PeerSessions::cleanup(
    const std::lock_guard<std::mutex> &lock)
{
    (void)lock;

    auto now = std::chrono::steady_clock::now();
    if (now < nextCleanup)
        return;

    auto iter = sessions.begin();
    while (iter != sessions.end())
    {
        PeerSession &session = iter->second;
        if (session.lastUse + std::chrono::seconds(SESSION_TIMEOUT_SECONDS) < now)
            iter = sessions.erase(iter);
        else
            ++iter;
    }
}
