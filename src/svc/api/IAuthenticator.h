#pragma once

#include <fluffycoin/svc/RequestScene.h>

#include <google/protobuf/any.pb.h>

namespace fluffycoin::svc
{

/**
 * Interface for authenticating API clients
 */
class IAuthenticator
{
    public:
        virtual ~IAuthenticator() = default;

        virtual void authenticateScene(
            RequestScene &scene,
            const google::protobuf::Any &auth) = 0;

        virtual void noAuth(
            RequestScene &scene) = 0;

        virtual void setResponseAuth(
            RequestScene &scene,
            google::protobuf::Any &auth) = 0;

    protected:
        IAuthenticator() = default;
        IAuthenticator(IAuthenticator &&) = default;
        IAuthenticator(const IAuthenticator &) = default;
        IAuthenticator &operator=(IAuthenticator &&) = default;
        IAuthenticator &operator=(const IAuthenticator &) = default;
};

}
