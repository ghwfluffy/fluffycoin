#pragma once

#include <fluffycoin/svc/IServiceParams.h>

namespace fluffycoin::svc
{

/**
 * Base implementation for IServiceParams that allows you to
 * create a child implementation without defining all of the methods.
 */
class AbstractServiceParams : public IServiceParams
{
    public:
        virtual ~AbstractServiceParams() = default;

        void initCmdLineParams(ArgParser &args) const override;
        void setCmdLineArgs(const Args &args) override;

        std::string getLogFile() const override;

        unsigned int getNumEventThreads() const override;

        uint16_t getApiPort() const override;
        uint16_t getEventPort() const override;

        const BinData &getServerKey() const override;

        void setupScene(ServiceScene &ctx) override;
        void addApiHandlers(ApiHandlerMap &handlers, bool paused) const override;
        void addEventSubscriptions(EventSubscriptionMap &handlers) const override;

        IAuthenticator *getAuthenticator() override;

        bool preInit() override;
        bool init() override;

        void cleanup() override;
        void postCleanup() override;

    protected:
        AbstractServiceParams() = default;
        AbstractServiceParams(AbstractServiceParams &&) = default;
        AbstractServiceParams(const AbstractServiceParams &) = default;
        AbstractServiceParams &operator=(AbstractServiceParams &&) = default;
        AbstractServiceParams &operator=(const AbstractServiceParams &) = default;
};

}
