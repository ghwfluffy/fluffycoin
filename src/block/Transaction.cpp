#include <fluffycoin/block/Transaction.h>

#include <fluffycoin/ossl/encode.h>

#include <fluffycoin/log/Log.h>

#include <openssl/asn1t.h>

namespace fluffycoin
{

namespace asn1
{

typedef struct TransactionChoice_st
{
    int type;
    union {
        ASN1_NULL *null;
        Transfer *transfer;
#if 0
        LotteryTicket *lotto;
        Donation *burn;
        MineAction *mine;
        StockAction *stock;
        VoteAction *vote;
        LoanAction *loan;
#endif
    } u;
} TransactionChoice;

DECLARE_ASN1_FUNCTIONS(TransactionChoice)

ASN1_CHOICE(TransactionChoice) =
{
    ASN1_SIMPLE(TransactionChoice, u.null, ASN1_NULL)
  , ASN1_IMP(TransactionChoice, u.transfer, Transfer, 1)
#if 0
  , ASN1_IMP(TransactionChoice, u.lotto, LotteryTicket, 2)
  , ASN1_IMP(TransactionChoice, u.burn, Donation, 3)
  , ASN1_IMP(TransactionChoice, u.mine, MineAction, 4)
  , ASN1_IMP(TransactionChoice, u.stock, StockAction, 5)
  , ASN1_IMP(TransactionChoice, u.vote, VoteAction, 6)
  , ASN1_IMP(TransactionChoice, u.loan, LoanAction, 7)
#endif
} ASN1_CHOICE_END(TransactionChoice)

IMPLEMENT_ASN1_FUNCTIONS(TransactionChoice)

typedef struct TransactionContent_st
{
    ASN1_INTEGER *time;
    TransactionChoice *action;
    ASN1_OCTET_STRING *signer;
    ASN1_OCTET_STRING *signerKey;
    ASN1_OCTET_STRING *newAddress;
} TransactionContent;

DECLARE_ASN1_FUNCTIONS(TransactionContent)

ASN1_SEQUENCE(TransactionContent) =
{
    ASN1_SIMPLE(TransactionContent, time, ASN1_INTEGER)
  , ASN1_SIMPLE(TransactionContent, action, TransactionChoice)
  , ASN1_SIMPLE(TransactionContent, signer, ASN1_OCTET_STRING)
  , ASN1_SIMPLE(TransactionContent, signerKey, ASN1_OCTET_STRING)
  , ASN1_SIMPLE(TransactionContent, newAddress, ASN1_OCTET_STRING)
} ASN1_SEQUENCE_END(TransactionContent)

IMPLEMENT_ASN1_FUNCTIONS(TransactionContent)

typedef struct Transaction_st
{
    TransactionContent *content;
    ASN1_OCTET_STRING *signature;
} Transaction;

ASN1_SEQUENCE(Transaction) =
{
    ASN1_SIMPLE(Transaction, content, TransactionContent)
  , ASN1_SIMPLE(Transaction, signature, ASN1_OCTET_STRING)
} ASN1_SEQUENCE_END(Transaction)

IMPLEMENT_ASN1_FUNCTIONS(Transaction)

}

}

using namespace fluffycoin;
using namespace fluffycoin::block;

Transaction::Transaction()
{
    resetChoice();
}

Transaction::Transaction(Transaction &&rhs)
{
    operator=(std::move(rhs));
}

Transaction &Transaction::operator=(Transaction &&rhs)
{
    if (this != &rhs)
    {
        resetChoice();

        this->type = rhs.type;
        this->transfer = std::move(rhs.transfer);
#if 0
        this->lotto = std::move(rhs.lotto);
        this->burn = std::move(rhs.burn);
        this->mine = std::move(rhs.mine);
        this->stock = std::move(rhs.stock);
        this->vote = std::move(rhs.vote);
        this->loan = std::move(rhs.loan);
#endif

        rhs.resetChoice();
    }

    return (*this);
}

void Transaction::resetChoice()
{
    type = Action::None;
    transfer.reset();
#if 0
    lotto.reset();
    burn.reset();
    mine.reset();
    stock.reset();
    vote.reset();
    loan.reset();
#endif
}

Transaction::Action Transaction::getAction() const
{
    return type;
}

const std::unique_ptr<Transfer> &Transaction::getTransfer() const
{
    return transfer;
}

void Transaction::setTransfer(Transfer transfer)
{
    resetChoice();
    type = Action::Transfer;
    this->transfer = std::make_unique<Transfer>(std::move(transfer));
}

#if 0
const std::unique_ptr<LotteryTicket> &Transaction::getLotto() const
{
    return lotto;
}

void Transaction::setLotto(LotteryTicket lotto)
{
    resetChoice();
    type = Action::Lotto;
    this->lotto = std::make_unique<LotteryTransfer>(std::move(lotto));
}

const std::unique_ptr<Donation> &Transaction::getDonation() const
{
    return burn;
}

void Transaction::setDonation(Donation donation)
{
    resetChoice();
    type = Action::Donation;
    this->burn = std::make_unique<Donation>(std::move(donation));
}

const std::unique_ptr<MineAction> &Transaction::getMine() const
{
    return mine;
}

void Transaction::setMine(MineAction mine)
{
    resetChoice();
    type = Action::Mine;
    this->mine = std::make_unique<MineAction>(std::move(mine));
}

const std::unique_ptr<StockAction> &Transaction::getStock() const
{
    return stock;
}

void Transaction::setStock(StockAction stock)
{
    resetChoice();
    type = Action::Stock;
    this->stock = std::make_unique<StockAction>(std::move(stock));
}

const std::unique_ptr<VoteAction> &Transaction::getVote() const
{
    return vote;
}

void Transaction::setVote(VoteAction vote)
{
    resetChoice();
    type = Action::Vote;
    this->vote = std::make_unique<VoteAction>(std::move(vote));
}

const std::unique_ptr<LoanAction> &Transaction::getLoan() const
{
    return loan;
}

void Transaction::setLoan(LoanAction loan)
{
    resetChoice();
    type = Action::Loan;
    this->loan = std::make_unique<LoanAction>(std::move(loan));
}
#endif

const Time &Transaction::getTime() const
{
    return time;
}

void Transaction::setTime(Time time)
{
    this->time = std::move(time);
}

const Address &Transaction::getSigner() const
{
    return signer;
}

void Transaction::setSigner(Address signer)
{
    this->signer = std::move(signer);
}

const PublicKey &Transaction::getSignerKey() const
{
    return signerKey;
}

void Transaction::setSignerKey(PublicKey key)
{
    this->signerKey = std::move(key);
}

const Address &Transaction::getNewAddress() const
{
    return newAddress;
}

void Transaction::setNewAddress(Address newAddress)
{
    this->newAddress = std::move(newAddress);
}

const Signature &Transaction::getSignature() const
{
    return signature;
}

void Transaction::setSignature(Signature signature)
{
    this->signature = std::move(signature);
}

void Transaction::toAsn1(asn1::Transaction &t) const
{
    asn1::TransactionChoice &choice = *t.content->action;

    // Cleanup any current content choice memory
    switch (static_cast<Transaction::Action>(choice.type))
    {
        case Action::None:
            ASN1_NULL_free(choice.u.null);
            choice.u.null = nullptr;
            break;
        case Action::Transfer:
            asn1::Transfer_free(choice.u.transfer);
            choice.u.transfer = nullptr;
            break;
#if 0
        case Action::Lotto:
            asn1::LotteryTicket_free(choice.u.lotto);
            choice.u.lotto = nullptr;
            break;
        case Action::Donation:
            asn1::Donation_free(choice.u.burn);
            choice.u.burn = nullptr;
            break;
        case Action::Mine:
            asn1::MineAction_free(choice.u.mine);
            choice.u.mine = nullptr;
            break;
        case Action::Stock:
            asn1::StockAction_free(choice.u.stock);
            choice.u.stock = nullptr;
            break;
        case Action::Vote:
            asn1::VoteAction_free(choice.u.vote);
            choice.u.vote = nullptr;
            break;
        case Action::Loan:
            asn1::LoanAction_free(choice.u.loan);
            choice.u.loan = nullptr;
            break;
#endif
        default:
            if (choice.u.null)
                log::error("Invalid toAsn1 cleanup transaction type {}.", choice.type);
            break;
    }

    // Set new content choice memory
    choice.type = static_cast<int>(type);
    switch (type)
    {
        case Action::None:
            choice.u.null = ASN1_NULL_new();
            break;
        case Action::Transfer:
            choice.u.transfer = asn1::Transfer_new();
            transfer->toAsn1(*choice.u.transfer);
            break;
#if 0
        case Action::Lotto:
            choice.u.lotto = asn1::LotteryTicket_new();
            lotto->toAsn1(*choice.u.lotto);
            break;
        case Action::Donation:
            choice.u.burn = asn1::Donation_new();
            burn->toAsn1(*choice.u.burn);
            break;
        case Action::Mine:
            choice.u.mine = asn1::MineAction_new();
            mine->toAsn1(*choice.u.mine);
            break;
        case Action::Stock:
            choice.u.stock = asn1::StockAction_new();
            stock->toAsn1(*choice.u.stock);
            break;
        case Action::Vote:
            choice.u.vote = asn1::VoteAction_new();
            vote->toAsn1(*choice.u.vote);
            break;
        case Action::Loan:
            choice.u.loan = asn1::LoanAction_new();
            loan->toAsn1(*choice.u.loan);
            break;
#endif
        default:
            log::error("Invalid toAsn1 transaction type {}.", static_cast<int>(type));
            break;
    }

    time.toAsn1(*t.content->time);
    signer.toAsn1(*t.content->signer);
    signerKey.toAsn1(*t.content->signerKey);
    newAddress.toAsn1(*t.content->newAddress);
    signature.toAsn1(*t.signature);
}

void Transaction::fromAsn1(const asn1::Transaction &t)
{
    resetChoice();

    const asn1::TransactionChoice &choice = *t.content->action;
    switch (static_cast<Transaction::Action>(choice.type))
    {
        case Action::None:
            break;
        case Action::Transfer:
            type = Action::Transfer;
            transfer = std::make_unique<Transfer>();
            transfer->fromAsn1(*choice.u.transfer);
            break;
#if 0
        case Action::Lotto:
            type = Action::Lotto;
            lotto = std::make_unique<LotteryTocket>();
            lotto->fromAsn1(*choice.u.lotto);
            break;
        case Action::Donation:
            type = Action::Donation;
            burn = std::make_unique<Donation>();
            burn->fromAsn1(*choice.u.burn);
            break;
        case Action::Mine:
            type = Action::Mine;
            mine = std::make_unique<MineAction>();
            mine->fromAsn1(*choice.u.mine);
            break;
        case Action::Stock:
            type = Action::Stock;
            stock = std::make_unique<StockAction>();
            stock->fromAsn1(*choice.u.stock);
            break;
        case Action::Vote:
            type = Action::Vote;
            vote = std::make_unique<VoteAction>();
            vote->fromAsn1(*choice.u.vote);
            break;
        case Action::Loan:
            type = Action::Loan;
            loan = std::make_unique<LoanAction>();
            loan->fromAsn1(*choice.u.loan);
            break;
#endif
        default:
            log::error("Invalid fromAsn1 transaction type {}.", choice.type);
            break;
    }

    time.fromAsn1(*t.content->time);
    signer.fromAsn1(*t.content->signer);
    signerKey.fromAsn1(*t.content->signerKey);
    newAddress.fromAsn1(*t.content->newAddress);
    signature.fromAsn1(*t.signature);
}

BinData Transaction::toContent() const
{
    asn1::Transaction *obj = asn1::Transaction_new();
    toAsn1(*obj);
    BinData data = ossl::encode(*obj->content, asn1::i2d_TransactionContent);
    asn1::Transaction_free(obj);
    return data;
}
