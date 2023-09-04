#pragma once

#include <fluffycoin/curator/fs/BlockStorage.h>

#include <fluffycoin/svc/AbstractServiceParams.h>

#include <fluffycoin/db/Database.h>

#include <memory>

namespace fluffycoin::curator
{

/**
 * Curator service is responsible for integrating new blocks into the database
 */
class CuratorServiceParams : public svc::AbstractServiceParams
{
    public:
        CuratorServiceParams();
        CuratorServiceParams(CuratorServiceParams &&) = delete;
        CuratorServiceParams(const CuratorServiceParams &) = delete;
        CuratorServiceParams &operator=(CuratorServiceParams &&) = delete;
        CuratorServiceParams &operator=(const CuratorServiceParams &) = delete;
        ~CuratorServiceParams() final = default;

        static constexpr const uint16_t API_PORT = 0x4643;
        static constexpr const uint16_t EVENT_PORT = 0x4663;

        std::string getLogFile() const final;

        void setupScene(svc::ServiceScene &ctx) final;

        void initCmdLineParams(ArgParser &args) const final;
        void setCmdLineArgs(const Args &args) final;

        uint16_t getApiPort() const final;
        uint16_t getEventPort() const final;

#if 0
        void addApiHandlers(svc::ApiHandlerMap &handlers, bool paused) const final;
        void addEventSubscriptions(svc::EventSubscriptionMap &handlers) const final;
#endif

        bool init(bool paused) final;
#if 0
        bool preInit(bool paused) final;

        void cleanup() final;
        void postCleanup() final;
#endif

    private:
        bool paused;
        uint64_t reloadFrom;
        std::string fsStorageDir;

        const svc::ServiceScene *pctx;
        std::unique_ptr<db::Database> db;
        fs::BlockStorage fsBlocks;
};

}
