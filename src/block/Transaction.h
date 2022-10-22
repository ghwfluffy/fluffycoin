#pragma once

#include <fluffycoin/block/Time.h>
#include <fluffycoin/block/Address.h>
#include <fluffycoin/block/Transfer.h>
#include <fluffycoin/block/PublicKey.h>
#include <fluffycoin/block/Signature.h>

#if 0
#include <fluffycoin/block/LotteryTicket.h>
#include <fluffycoin/block/Donation.h>
#include <fluffycoin/block/MineAction.h>
#include <fluffycoin/block/StockAction.h>
#include <fluffycoin/block/VoteAction.h>
#include <fluffycoin/block/LoanAction.h>
#endif

#include <fluffycoin/utils/BinData.h>

#include <fluffycoin/ossl/convert.h>

#include <memory>

namespace fluffycoin
{

namespace asn1
{
    typedef struct Transaction_st Transaction;
    DECLARE_ASN1_FUNCTIONS(Transaction)
    FLUFFYCOIN_DEFINE_STACK_FUNCTIONS(Transaction)
}

namespace block
{

/**
 * An individual transaction by a wallet. Anyone can create one
 * and send it to the p2p network to be included in an upcoming
 * node block for the required fee.
 */
class Transaction
{
    public:
        Transaction();
        Transaction(Transaction &&);
        Transaction(const Transaction &) = delete;
        Transaction &operator=(Transaction &&);
        Transaction &operator=(const Transaction &) = delete;
        ~Transaction() = default;

        enum class Action
        {
            None = 0,
            Transfer,
#if 0
            Lotto,
            Donation,
            Mine,
            Stock,
            Vote,
            Loan,
#endif
        };

        Action getAction() const;

        const std::unique_ptr<Transfer> &getTransfer() const;
        void setTransfer(Transfer transfer);

#if 0
        const std::unique_ptr<LotteryTicket> &getLotto() const;
        void setLotto(LotteryTicket lotto);

        const std::unique_ptr<Donation> &getDonation() const;
        void setDonation(Donation donation);

        const std::unique_ptr<MineAction> &getMine() const;
        void setMine(MineAction mine);

        const std::unique_ptr<StockAction> &getStock() const;
        void setStock(StockAction stock);

        const std::unique_ptr<VoteAction> &getVote() const;
        void setVote(VoteAction vote);

        const std::unique_ptr<LoanAction> &getLoan() const;
        void setLoan(LoanAction loan);
#endif

        const Time &getTime() const;
        void setTime(Time time);

        const Address &getSigner() const;
        void setSigner(Address signer);

        const PublicKey &getSignerKey() const;
        void setSignerKey(PublicKey key);

        const Address &getNewAddress() const;
        void setNewAddress(Address newAddress);

        const Signature &getSignature() const;
        void setSignature(Signature signature);

        void toAsn1(asn1::Transaction &) const;
        void fromAsn1(const asn1::Transaction &);

        BinData toContent() const;

    private:
        void resetChoice();

        Action type;

        Time time;
        Address signer;
        PublicKey signerKey;
        Address newAddress;
        Signature signature;

        std::unique_ptr<Transfer> transfer;
#if 0
        std::unique_ptr<LotteryTicket> lotto;
        std::unique_ptr<Donation> burn;
        std::unique_ptr<MineAction> mine;
        std::unique_ptr<StockAction> stock;
        std::unique_ptr<VoteAction> vote;
        std::unique_ptr<LoanAction> loan;
#endif
};

}

}
