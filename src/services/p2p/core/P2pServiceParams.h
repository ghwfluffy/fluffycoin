#pragma once

#include <fluffycoin/svc/AbstractServiceParams.h>

#include <fluffycoin/p2p/BruteForce.h>
#include <fluffycoin/p2p/StakeKey.h>

namespace fluffycoin
{

namespace p2p
{

/**
 * Setup the p2p service
 */
class P2pServiceParams : public svc::AbstractServiceParams
{
    public:
        P2pServiceParams();
        P2pServiceParams(P2pServiceParams &&) = delete;
        P2pServiceParams(const P2pServiceParams &) = delete;
        P2pServiceParams &operator=(P2pServiceParams &&) = delete;
        P2pServiceParams &operator=(const P2pServiceParams &) = delete;
        ~P2pServiceParams() final = default;

        static constexpr const uint16_t API_PORT = 0x4643;
        static constexpr const uint16_t EVENT_PORT = 0x4663;
        static constexpr const char *WALLET_PW_ENV = "FLUFFYCOIN_WALLET_PASSWORD";

        std::string getLogFile() const final;

        void setupScene(svc::ServiceScene &ctx) final;

        void initCmdLineParams(ArgParser &args) const final;
        void setCmdLineArgs(const Args &args) final;

        uint16_t getApiPort() const final;
        uint16_t getEventPort() const final;

        const BinData &getServerKey() const final;

        void addApiHandlers(svc::ApiHandlerMap &handlers, bool paused) const final;
#if 0
        void addEventSubscriptions(svc::EventSubscriptionMap &handlers) const final;
#endif

        svc::IAuthenticator *getAuthenticator() final;

        bool preInit(bool paused) final;
#if 0
        bool init(bool paused) final;

        void cleanup() final;
        void postCleanup() final;
#endif

    private:
        StakeKey stakeKey;
        BruteForce bruteForce;

        // Configuration
        std::string walletFile;
        SafeData walletPassword;
        std::string stakeAddress;
};

}

}
