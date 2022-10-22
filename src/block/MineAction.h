#pragma once

#include <fluffycoin/block/Register.h>
#include <fluffycoin/block/Unregister.h>
#include <fluffycoin/block/CredentialChange.h>

#include <openssl/asn1.h>

#include <memory>

namespace fluffycoin
{

namespace asn1
{
    typedef struct MineAction_st MineAction;
    DECLARE_ASN1_FUNCTIONS(MineAction)
}

namespace block
{

/**
 * Modify who is a validator of the blockchain
 */
class MineAction
{
    public:
        MineAction();
        MineAction(MineAction &&);
        MineAction(const MineAction &) = delete;
        MineAction &operator=(MineAction &&);
        MineAction &operator=(const MineAction &) = delete;
        ~MineAction() = default;

        enum class Action : int
        {
            None = 0,
            Register,
            Unregister,
            CredentialChange,
        };

        Action getAction() const;

        const std::unique_ptr<Register> &getRegister() const;
        void setRegister(Register tregister);

        const std::unique_ptr<Unregister> &getUnregister() const;
        void setUnregister(Unregister unregister);

        const std::unique_ptr<CredentialChange> &getCredentialChange() const;
        void setCredentialChange(CredentialChange credentialChange);

        void toAsn1(asn1::MineAction &) const;
        void fromAsn1(const asn1::MineAction &);

    private:
        void resetChoice();

        Action action;
        std::unique_ptr<Register> tregister;
        std::unique_ptr<Unregister> unregister;
        std::unique_ptr<CredentialChange> credentialChange;
};

}

}
