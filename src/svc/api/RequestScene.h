#pragma once

#include <fluffycoin/svc/ServiceScene.h>
#include <fluffycoin/svc/Log.h>

#include <fluffycoin/utils/Details.h>

#include <google/protobuf/message.h>

#include <fmt/format.h>

#include <memory>

namespace fluffycoin::svc
{

/**
 * Context and memory management for handling an API request
 */
class RequestScene : public async::Treasure
{
    public:
        RequestScene(
            const ServiceScene &svcScene);
        RequestScene(RequestScene &&) = default;
        RequestScene(const RequestScene &) = delete;
        RequestScene &operator=(RequestScene &&) = default;
        RequestScene &operator=(const RequestScene &) = delete;
        ~RequestScene() = default;

        void setRequest(std::unique_ptr<google::protobuf::Message> msg);

        template<typename T>
        T &req()
        {
            return *static_cast<T *>(msg.get());
        }

        Details &details();
        log::Logger &log();
        const svc::ServiceScene &utils() const;

        bool isOk() const;

        const ErrorStatus &getError() const;

        template<typename... Args>
        void setError(ErrorCode code, const char *field, fmt::format_string<Args...> s, Args&&... args)
        {
            deets.setError(svc::Log::Api, code, field, std::move(s), std::forward<Args>(args)...);
        }

        template<typename Category, typename... Args>
        void setError(Category cat, ErrorCode code, const char *field, fmt::format_string<Args...> s, Args&&... args)
        {
            deets.setError(cat, code, field, std::move(s), std::forward<Args>(args)...);
        }

        void operator+=(Details &details);

    private:
        Details deets;
        const ServiceScene *svcScene;
        std::unique_ptr<google::protobuf::Message> msg;
};

}
