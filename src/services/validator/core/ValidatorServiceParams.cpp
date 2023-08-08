#include <fluffycoin/validator/ValidatorServiceParams.h>

#include <fluffycoin/validator/api/server/Handshake.h>

#include <fluffycoin/alg/Wallet.h>

#include <fluffycoin/utils/FileTools.h>

using namespace fluffycoin;
using namespace fluffycoin::validator;

ValidatorServiceParams::ValidatorServiceParams()
{
    walletFile = "~/.fluffycoin/wallet.json";
}

std::string ValidatorServiceParams::getLogFile() const
{
    return "/var/log/fluffycoin/validator.log";
}

void ValidatorServiceParams::setupScene(svc::ServiceScene &ctx)
{
    ctx.set(stakeKey);
    ctx.set(bruteForce);
}

void ValidatorServiceParams::initCmdLineParams(ArgParser &args) const
{
    args.addParam('w', "wallet", "Wallet containing stake key");
    args.addParam('p', "wallet-password", "Wallet password\n"
                                          "Default: Use FLUFFYCOIN_WALLET_PASSWORD env var");
    args.addParam('a', "stake-address", "Address from wallet matching stake key\n"
                                        "Default: Newest key in wallet");
}

void ValidatorServiceParams::setCmdLineArgs(const Args &args)
{
    if (args.hasArg("wallet"))
        walletFile = args.getArg("wallet");

    if (args.hasArg("stake-address"))
        stakeAddress = args.getArg("stake-address");

    if (args.hasArg("wallet-password"))
    {
        walletPassword = SafeData(args.getArg("wallet-password"));
        const_cast<Args &>(args).maskArg("wallet-address");
    }
    else
    {
        const char *pw = getenv(WALLET_PW_ENV);
        if (pw)
        {
            walletPassword.setData(reinterpret_cast<const unsigned char *>(pw), strlen(pw));
            setenv(WALLET_PW_ENV, "", true);
        }
    }   
}

uint16_t ValidatorServiceParams::getApiPort() const
{
    return API_PORT;
}

uint16_t ValidatorServiceParams::getEventPort() const
{
    return EVENT_PORT;
}

const BinData &ValidatorServiceParams::getServerKey() const
{
    return stakeKey.getPriv();
}

void ValidatorServiceParams::addApiHandlers(svc::ApiHandlerMap &handlers, bool paused) const
{
    if (paused)
        return;

    handlers.add<fcpb::p2p::v1::auth::AuthenticateSession>(api::server::Handshake::process);
}

#if 0
void ValidatorServiceParams::addEventSubscriptions(svc::EventSubscriptionMap &handlers) const
{
    // TODO
}
#endif

svc::IAuthenticator *ValidatorServiceParams::getAuthenticator()
{
    // TODO
    return nullptr;
}

bool ValidatorServiceParams::preInit()
{
    // Read in wallet data
    std::string walletContents;
    if (!FileTools::read(walletFile, walletContents))
    {
        log::error("Failed to read in wallet data from '{}'.", walletFile);
        return false;
    }

    // Parse wallet
    alg::Wallet stakeWallet;
    if (!stakeWallet.setString(walletContents, walletPassword))
    {
        log::error("Failed to open wallet file.");
        return false;
    }

    // Find stake key
    ossl::EvpPkeyPtr key;
    if (!stakeAddress.empty())
        key = stakeWallet.getKey(stakeAddress);
    else
    {
        key = stakeWallet.getLatestKey(alg::Wallet::KeyUsage::Validator);
        stakeAddress = stakeWallet.getLatestAddress(alg::Wallet::KeyUsage::Validator);
    }

    if (stakeAddress.empty() || !key)
    {
        log::error("Failed to find stake key '{}'.", stakeAddress);
        return false;
    }

    stakeKey.setKey(std::move(stakeAddress), std::move(key));

    return true;
}

#if 0
bool ValidatorServiceParams::init()
{
    // TODO
    return false;
}

void ValidatorServiceParams::cleanup()
{
    // TODO
}

void ValidatorServiceParams::postCleanup()
{
    // TODO
}
#endif
