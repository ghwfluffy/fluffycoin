#include <fluffycoin/svc/Service.h>
#include <fluffycoin/svc/LogArgs.h>
#include <fluffycoin/svc/Log.h>

#include <fluffycoin/ossl/Entropy.h>

#include <chrono>

#include <unistd.h>
#include <signal.h>

using namespace fluffycoin;
using namespace fluffycoin::svc;

Service::Service()
        : status(ServiceStatusCode::Running)
        , ctx(
            std::bind(&Service::setStatus, this, std::placeholders::_1),
            scheduler.asio(),
            treasureTrove)
{
    argc = 0;
    argv = nullptr;
    paused = false;

    bArgsInit = false;
    bPreInit = false;
    bInit = false;
    bPostInit = false;

    bRandInit = false;
    bSignalsInit = false;
    bSceneSetup = false;
    interrupts = 0;

    ctx.set(zmqCtx);

    svc::initLogger();
}

Service::~Service()
{
    stopService();
}

fluffycoin::svc::ServiceStatusCode Service::exec()
{
    // Main service loop
    while (status == ServiceStatusCode::Running)
    {
        // Try to initialize
        bool bFirstInit = !bPostInit;
        if (!init())
        {
            constexpr const unsigned int FAILED_INIT_SLEEP_SECONDS = 5;
            sleep(FAILED_INIT_SLEEP_SECONDS);
            continue;
        }

        // Log after initialized
        if (bFirstInit)
            log::info("Service initialized.");

        // Wait till we are supposed to shut down
        std::unique_lock<std::mutex> lock(mainLoopMutex);
        if (status == ServiceStatusCode::Running)
        {
            constexpr const unsigned int MAIN_LOOP_IDLE_SECONDS = 2;
            mainLoopCondition.wait_for(lock, std::chrono::seconds(MAIN_LOOP_IDLE_SECONDS));
        }
    }

    // Stop threads and run cleanup routines
    cleanup();

    return status.load();
}

void Service::setArgs(
    int argc,
    const char **argv)
{
    this->argc = argc;
    this->argv = argv;
}

void Service::setPaused(
    bool paused)
{
    this->paused = paused;
}

void Service::setParams(
    std::unique_ptr<IServiceParams> params)
{
    this->params = std::move(params);
}

void Service::setStatus(
    ServiceStatusCode code)
{
    this->status = code;
    std::unique_lock<std::mutex> lock(mainLoopMutex);
    mainLoopCondition.notify_all();
}

bool Service::init()
{
    if (!params)
        return log::error("No service parameters initialized.");

    return
        // Setup signal handlers
        (bSignalsInit || (bSignalsInit = initSignals())) &&
        // Handle command line arguments
        (bArgsInit || (bArgsInit = initArgs())) &&
        // Add entropy to the DRBG
        (bRandInit || (bRandInit = initRand())) &&
        // Setup service dependencies
        (bSceneSetup || (bSceneSetup = setupScene())) &&
        // Service-specific pre-initialization
        (bPreInit || (bPreInit = params->preInit())) &&
        // Setup ports and threads
        (bInit || (bInit = startService())) &&
        // Service-specific post-initialization
        (bPostInit || (bPostInit = params->init()));
}

void Service::cleanup()
{
    // Service-specific pre-deinitialization
    if (params)
        params->cleanup();

    // Stop ports and threads
    stopService();

    // Service-specific post-deinitialization
    if (params)
        params->postCleanup();

    // Reset signal handlers
    cleanupSignals();
}

namespace
{

struct SignalState
{
    unsigned int &interrupts;
    std::function<void(ServiceStatusCode)> setStatus;

    SignalState(
        unsigned int &interrupts,
        std::function<void(ServiceStatusCode)> setStatus)
            : interrupts(interrupts)
            , setStatus(setStatus)
    {}
};

std::unique_ptr<SignalState> signalState;
void handleSignal(int signal)
{
    constexpr const unsigned int MAX_INTERRUPTS = 5;
    if (signal == SIGINT)
    {
        if (signalState->interrupts++ == 0)
        {
            log::info("Interrupt signature received. Shutting down gracefully.");
            signalState->setStatus(ServiceStatusCode::Shutdown);
        }
        else if (signalState->interrupts >= MAX_INTERRUPTS)
        {
            log::info("Immediate shutdown requested. Terminating immediately.");
            _Exit(128);
        }
    }
    else
    {
        log::info("Terminate signal received. Shutting down gracefully.");
        signalState->setStatus(ServiceStatusCode::Shutdown);
    }
}

}

bool Service::initSignals()
{
    signalState = std::make_unique<SignalState>(
        interrupts,
        std::bind(&Service::setStatus, this, std::placeholders::_1));

    // Catch SIGTERM/SIGINT
    struct sigaction handler = {};
    handler.sa_handler = handleSignal;
    sigaction(SIGTERM, &handler, nullptr);
    sigaction(SIGINT, &handler, nullptr);

    // Ignore SIGPIPE
    handler.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &handler, nullptr);

    return true;
}

void Service::cleanupSignals()
{
    struct sigaction handler = {};
    handler.sa_handler = SIG_DFL;

    sigaction(SIGTERM, &handler, nullptr);
    sigaction(SIGINT, &handler, nullptr);
    sigaction(SIGPIPE, &handler, nullptr);

    signalState.reset();
}

bool Service::initArgs()
{
    // Have anything to do?
    if (argc <= 0 || !argv)
        return true;

    // Parse arguments
    ArgParser argParser;
    LogArgs::initCmdLineParams(argParser);
    params->initCmdLineParams(argParser);
    Args args = argParser.parse(argc, argv);

    // Set logging parameters from arguments
    LogArgs::setCmdLineArgs(args);
    if (LogArgs::logFile(args).empty())
        log::setFile(params->getLogFile());

    // Set service specific parameters
    params->setCmdLineArgs(args);

    // If we failed to parse command line arguments we will quit
    if (args.hasError())
        status = ServiceStatusCode::Shutdown;

    return !args.hasError();
}

bool Service::initRand()
{
    ossl::Entropy::seed();
    return true;
}

bool Service::setupScene()
{
    params->setupScene(ctx);
    return true;
}

bool Service::startService()
{
    bool bOk = true;

    // Start event handler
    scheduler.setThreads(params->getNumEventThreads());
    scheduler.start();

    // Setup event publisher
    if (bOk && params->getEventPort() > 0 && !paused)
    {
        eventPublisher = std::make_unique<EventPublisher>(zmqCtx);
        bOk = eventPublisher->openPort(params->getEventPort(), params->getServerKey());
        // Add it to the scene
        if (bOk)
            ctx.set(*eventPublisher);
    }

    // Setup API port
    if (bOk && params->getApiPort() > 0)
    {
        params->addApiHandlers(apiMap, paused);
        apiProcessor = std::make_unique<ApiPort>(ctx, zmqCtx, apiMap);
        apiProcessor->setAuthenticator(params->getAuthenticator());
        bOk = apiProcessor->openPort(params->getApiPort(), params->getServerKey());
    }

    // Subscribe to local microservice events and be ready to subscribe to peers
    if (bOk && !paused)
    {
        params->addEventSubscriptions(subscriptions);
        eventProcessor = std::make_unique<EventProcessor>(ctx, zmqCtx, subscriptions);
        bOk = eventProcessor->subscribeLocal();
        if (bOk)
            ctx.set(*eventProcessor);
    }

    return bOk;
}

void Service::stopService()
{
    // Stop processing events and API calls
    eventProcessor.reset();
    apiProcessor.reset();

    // Stop event handler
    scheduler.stop();

    // Cleanup event port
    eventPublisher.reset();
}
