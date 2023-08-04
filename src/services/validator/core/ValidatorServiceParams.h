#pragma once

#include <fluffycoin/svc/AbstractServiceParams.h>

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

        std::string getLogFile() const final;

#if 0
        void initCmdLineParams(ArgParser &args) const final;
        void setCmdLineArgs(const Args &args) final;

        unsigned int getNumEventThreads() const final;

        uint16_t getApiPort() const final;
        uint16_t getEventPort() const final;

        const BinData &getServerKey() const final;

        void setupScene(ServiceScene &ctx) final;
        void addApiHandlers(ApiHandlerMap &handlers, bool paused) const final;
        void addEventSubscriptions(EventSubscriptionMap &handlers) const final;

        IAuthenticator *getAuthenticator() final;

        bool preInit() final;
        bool init() final;

        void cleanup() final;
        void postCleanup() final;
#endif
};

}

}
