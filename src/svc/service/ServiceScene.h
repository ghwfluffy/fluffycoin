#pragma once

#include <fluffycoin/svc/ServiceStatusCode.h>

#include <fluffycoin/async/boost.h>
#include <fluffycoin/async/TreasureTrove.h>

#include <map>
#include <functional>

namespace fluffycoin::svc
{

/**
 * Dependency injectable service-wide context that is
 * personalized on microservice startup and gives access
 * to the internal services available to that microservice.
 *
 * This class should not be injected any further down than
 * classes that are specific to a certain microservice.
 */
struct ServiceScene
{
    ServiceScene(
        std::function<void(ServiceStatusCode)> statusCallback,
        boost::asio::io_context &asio,
        async::TreasureTrove &trove);
    ServiceScene(ServiceScene &&) = delete;
    ServiceScene(const ServiceScene &) = delete;
    ServiceScene &operator=(ServiceScene &&) = delete;
    ServiceScene &operator=(const ServiceScene &) = delete;
    ~ServiceScene() = default;

    boost::asio::io_context &asio;
    async::TreasureTrove &trove;

    template<typename T>
    T &get() const
    {
        return *reinterpret_cast<T *>(getPtr(typeid(T).hash_code()));
    }

    void setStatus(ServiceStatusCode);

    private:
    void *getPtr(size_t) const;
    void setPtr(size_t, void *);

    std::map<size_t, void *> utils;
    std::function<void(ServiceStatusCode)> statusUpdate;

    public:
    template<typename T>
    void set(T &t)
    {
        setPtr(typeid(T).hash_code(), &t);
    }
};

}
