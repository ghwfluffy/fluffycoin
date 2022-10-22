#include <fluffycoin/block/MineAction.h>

#include <fluffycoin/log/Log.h>

#include <openssl/asn1t.h>

namespace fluffycoin
{

namespace asn1
{

typedef struct MineAction_st
{
    int type;
    union {
        ASN1_NULL *null;
        Register *tregister;
        Unregister *unregister;
        CredentialChange *credentialChange;
    } u;
} MineAction;

ASN1_CHOICE(MineAction) =
{
    ASN1_SIMPLE(MineAction, u.null, ASN1_NULL)
  , ASN1_IMP(MineAction, u.tregister, Register, 1)
  , ASN1_IMP(MineAction, u.unregister, Unregister, 2)
  , ASN1_IMP(MineAction, u.credentialChange, CredentialChange, 3)
} ASN1_CHOICE_END(MineAction)

IMPLEMENT_ASN1_FUNCTIONS(MineAction)

}

}

using namespace fluffycoin;
using namespace fluffycoin::block;

/**
 * Modify who is a validator of the blockchain
 */
MineAction::MineAction()
{
    action = Action::None;
}

MineAction::MineAction(MineAction &&rhs)
{
    action = Action::None;
    operator=(std::move(rhs));
}

MineAction &MineAction::operator=(MineAction &&rhs)
{
    if (this != &rhs)
    {
        resetChoice();

        action = rhs.action;
        tregister = std::move(rhs.tregister);
        unregister = std::move(rhs.unregister);
        credentialChange = std::move(rhs.credentialChange);

        rhs.resetChoice();
    }

    return (*this);
}

MineAction::Action MineAction::getAction() const
{
    return action;
}

const std::unique_ptr<Register> &MineAction::getRegister() const
{
    return tregister;
}

void MineAction::setRegister(Register tregister)
{
    resetChoice();
    action = Action::Register;
    this->tregister = std::make_unique<Register>(std::move(tregister));
}

const std::unique_ptr<Unregister> &MineAction::getUnregister() const
{
    return unregister;
}

void MineAction::setUnregister(Unregister unregister)
{
    resetChoice();
    action = Action::Unregister;
    this->unregister = std::make_unique<Unregister>(std::move(unregister));
}

const std::unique_ptr<CredentialChange> &MineAction::getCredentialChange() const
{
    return credentialChange;
}

void MineAction::setCredentialChange(CredentialChange credentialChange)
{
    resetChoice();
    action = Action::CredentialChange;
    this->credentialChange = std::make_unique<CredentialChange>(std::move(credentialChange));
}

void MineAction::toAsn1(asn1::MineAction &t) const
{
    // Cleanup any current choice memory
    switch (static_cast<MineAction::Action>(t.type))
    {
        case Action::None:
            ASN1_NULL_free(t.u.null);
            t.u.null = nullptr;
            break;
        case Action::Register:
            asn1::Register_free(t.u.tregister);
            t.u.tregister = nullptr;
            break;
        case Action::Unregister:
            asn1::Unregister_free(t.u.unregister);
            t.u.unregister = nullptr;
            break;
        case Action::CredentialChange:
            asn1::CredentialChange_free(t.u.credentialChange);
            t.u.credentialChange = nullptr;
            break;
        default:
            if (t.u.null)
                log::error("Invalid toAsn1 cleanup mine action {}.", t.type);
            break;
    }

    // Set new choice memory
    t.type = static_cast<int>(action);
    switch (action)
    {
        case Action::None:
            t.u.null = ASN1_NULL_new();
            break;
        case Action::Register:
            t.u.tregister = asn1::Register_new();
            tregister->toAsn1(*t.u.tregister);
            break;
        case Action::Unregister:
            t.u.unregister = asn1::Unregister_new();
            unregister->toAsn1(*t.u.unregister);
            break;
        case Action::CredentialChange:
            t.u.credentialChange = asn1::CredentialChange_new();
            credentialChange->toAsn1(*t.u.credentialChange);
            break;
        default:
            log::error("Invalid toAsn1 mine action {}.", static_cast<int>(action));
            break;
    }
}

void MineAction::fromAsn1(const asn1::MineAction &t)
{
    resetChoice();

    switch (static_cast<MineAction::Action>(t.type))
    {
        case Action::None:
            break;
        case Action::Register:
            action = Action::Register;
            tregister = std::make_unique<Register>();
            tregister->fromAsn1(*t.u.tregister);
            break;
        case Action::Unregister:
            action = Action::Unregister;
            unregister = std::make_unique<Unregister>();
            unregister->fromAsn1(*t.u.unregister);
            break;
        case Action::CredentialChange:
            action = Action::CredentialChange;
            credentialChange = std::make_unique<CredentialChange>();
            credentialChange->fromAsn1(*t.u.credentialChange);
            break;
        default:
            log::error("Invalid fromAsn1 mine action {}.", t.type);
            break;
    }
}

void MineAction::resetChoice()
{
    action = Action::None;
    tregister.reset();
    unregister.reset();
    credentialChange.reset();
}
