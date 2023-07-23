#pragma once

#include <fluffycoin/svc/IServiceParams.h>
#include <fluffycoin/svc/ServiceStatusCode.h>
#include <fluffycoin/svc/ApiPort.h>
#include <fluffycoin/svc/EventPublisher.h>
#include <fluffycoin/svc/EventProcessor.h>
#include <fluffycoin/svc/ServiceScene.h>

#include <fluffycoin/async/Scheduler.h>
#include <fluffycoin/async/TreasureTrove.h>

#include <fluffycoin/zmq/Context.h>

#include <atomic>
#include <memory>

#include <mutex>
#include <condition_variable>

namespace fluffycoin::svc
{

/**
 * The top level singleton for a service that sets up all of the memory
 * and processing for a service.
 */
class Service
{
    public:
        template<typename Params>
        static ServiceStatusCode run(int argc, const char **argv, bool paused = false)
        {
            static_assert(
                std::is_base_of_v<Params, IServiceParams>,
                "Service requires IServiceParams to run.");

            Service svc;
            svc.setArgs(argc, argv);
            svc.setPaused(paused);
            svc.setParams(std::make_unique<Params>());
            return svc.exec();
        }

    private:
        Service();
        Service(Service &&) = delete;
        Service(const Service &) = delete;
        Service &operator=(Service &&) = delete;
        Service &operator=(const Service &) = delete;
        ~Service();

        ServiceStatusCode exec();

        void setArgs(
            int argc,
            const char **argv);

        void setPaused(
            bool paused);

        void setParams(
            std::unique_ptr<IServiceParams> params);

        void setStatus(
            ServiceStatusCode code);

        bool init();
        void cleanup();

        bool initSignals();
        void cleanupSignals();

        bool initArgs();
        bool initRand();
        bool setupScene();

        bool startService();
        void stopService();

        int argc;
        const char **argv;
        bool paused;
        std::unique_ptr<IServiceParams> params;

        std::atomic<ServiceStatusCode> status;
        async::Scheduler scheduler;
        async::TreasureTrove treasureTrove;
        zmq::Context zmqCtx;

        ServiceScene ctx;

        std::mutex mainLoopMutex;
        std::condition_variable mainLoopCondition;

        bool bRandInit;
        bool bSignalsInit;
        bool bSceneSetup;
        bool bArgsInit;
        bool bPreInit;
        bool bInit;
        bool bPostInit;
        unsigned int interrupts;

        ApiHandlerMap apiMap;
        EventSubscriptionMap subscriptions;

        std::unique_ptr<ApiPort> apiProcessor;
        std::unique_ptr<EventPublisher> eventPublisher;
        std::unique_ptr<EventProcessor> eventProcessor;
};

}
