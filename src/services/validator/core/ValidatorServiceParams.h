#pragma once

#include <fluffycoin/svc/AbstractServiceParams.h>

#include <fluffycoin/validator/BruteForce.h>
#include <fluffycoin/validator/StakeKey.h>

namespace fluffycoin
{

namespace validator
{

/**
 * Setup the validator service
 */
class ValidatorServiceParams : public svc::AbstractServiceParams
{
    public:
        ValidatorServiceParams();
        ValidatorServiceParams(ValidatorServiceParams &&) = delete;
        ValidatorServiceParams(const ValidatorServiceParams &) = delete;
        ValidatorServiceParams &operator=(ValidatorServiceParams &&) = delete;
        ValidatorServiceParams &operator=(const ValidatorServiceParams &) = delete;
        ~ValidatorServiceParams() final = default;

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
