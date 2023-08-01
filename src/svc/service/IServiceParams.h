#pragma once

#include <fluffycoin/svc/ServiceScene.h>

#include <fluffycoin/svc/ApiHandlerMap.h>
#include <fluffycoin/svc/IAuthenticator.h>
#include <fluffycoin/svc/EventSubscriptionMap.h>

#include <fluffycoin/utils/BinData.h>
#include <fluffycoin/utils/ArgParser.h>

#include <stdint.h>

namespace fluffycoin::svc
{

/**
 * Interface that performs all of the service-specific setup and configuration.
 */
class IServiceParams
{
    public:
        virtual ~IServiceParams() = default;

        virtual void initCmdLineParams(ArgParser &args) const = 0;
        virtual void setCmdLineArgs(const Args &args) = 0;

        virtual std::string getLogFile() const = 0;

        virtual unsigned int getNumEventThreads() const = 0;

        virtual uint16_t getApiPort() const = 0;
        virtual uint16_t getEventPort() const = 0;

        virtual const BinData &getServerKey() const = 0;

        virtual void setupScene(ServiceScene &ctx) = 0;
        virtual void addApiHandlers(ApiHandlerMap &handlers, bool paused) const = 0;
        virtual void addEventSubscriptions(EventSubscriptionMap &handlers) const = 0;

        virtual IAuthenticator *getAuthenticator() = 0;

        virtual bool preInit() = 0;
        virtual bool init() = 0;

        virtual void cleanup() = 0;
        virtual void postCleanup() = 0;

    protected:
        IServiceParams() = default;
        IServiceParams(IServiceParams &&) = default;
        IServiceParams(const IServiceParams &) = default;
        IServiceParams &operator=(IServiceParams &&) = default;
        IServiceParams &operator=(const IServiceParams &) = default;
};

}
