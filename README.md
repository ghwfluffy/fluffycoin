# <img src="https://fluffyco.in/fc-small.png" height="40" /> Fluffycoin Decentralized Consensus Protocol

## Background

### What is Fluffycoin?

Fluffycoin is a decentralized indie-blockchain, a peer-to-peer protocol, a mechanism for recording verifiable agreements, but mostly an art project. It's a new layer 1 blockchain with a custom protocol and consensus algorithm. A fluffycoin is the unit of authority on the chain, and is subdivided into 100 million fluffs.

### Why?

Why build a brand new blockchain? Surely making a layer 2 on top of Etherium, a Parachain on Polkadot, or a dapp on Solana would have more potential. Because it's fun. This requires a lot of complicated problems to solve, and it requires solving them with code. A lot of code. Code is my brush and vim is my canvas. I consider a fluffycoin to be more like an NFT than a cryptocurrency. If you hold a fluffycoin then you have a piece of art.

### This is not Fluffy \<space> Coin

A YouTuber named Biaheza used the name "Fluffy Coin" (with a space) to create an ERC-20 smart contract on the Etherium blockchain for a video to explain how easy it is to setup an alt coin. I've been all talk and no code on this project since 2018, so I can only blame myself for letting the name get minted by someone else in late 2021.

Many other names were considered but nothing else felt right. I'll give Biaheza some coins when the public network goes live and let him know I'm using the name. Check out his content on YouTube, I've been watching him for years.

## Status

Project is still under construction. Most of the work completed is the core infrastructure for the code base. Recently started tackling problems related to the actual blockchain, like serializing the current state of the blockchain to a database for fast validation of new blocks.

Some very high level milestones are described below. The list will expand over time. This is not intended to be a professional project with a comprehensive design. I am just attacking problems and changing requirements as I feel like it.

<div style="margin-left: auto; margin-right: auto; width: 30%">

| Component | Status |
| :--------: | :----: |
| Twitch stream setup<BR/>https://twitch.tv/GhwFluffy | ✅ Complete |
| High level design | ✅ Complete |
| Build system | ✅ Complete |
| Docker runtime | ✅ Complete |
| Testing infrastructure | ✅ Complete |
| C++ core/crypto utilities | ✅ Complete |
| Async coroutine architecture | ✅ Complete |
| Async database interface | ✅ Complete |
| API and Pub/Sub architecture | ✅ Complete |
| Microservice architecture | ✅ Complete |
| Block serialization | ✅ Complete |
| Genesis CLI | ✅ Complete |
| Database model | 🟠 In Progress |
| P2P discovery & auth | ⭕ Pending |
| fluffyco.in registry | ⭕ Pending |
| Block builder | ⭕ Pending |
| Byzantine validator voting | ⭕ Pending |
| Staking | ⭕ Pending |
| Transfer transactions | ⭕ Pending |
| Transactions CLI | ⭕ Pending |
| Alpha network | ⭕ Pending |
| Lotteries | ⭕ Pending |
| Trinkets | ⭕ Pending |
| Social voting | ⭕ Pending |
| Liquid staking | ⭕ Pending |
| Stocks | ⭕ Pending |
| Public network | ⭕ Pending |

</div>

## Protocol Features

### Proof of Stake

The proof of stake algorithm allows you to take a cut of the transaction fees and requires setting up a validator node. Validator nodes communicate with eachother using a gossip protocol. They negotiate a consensus on the order of who gets to build the next block. The more you stake the more often you will be elected to build the next block.

The validator that builds the block receives the largest portion of the transaction fees. All validators receive a small portion of the transaction fees. And another small percentage of the transaction fees get burned (not given to anyone).

Validators are penalized for building invalid blocks, affirming invalid blocks, missing their turn to build a block, or not affirming enough blocks built by other validators.

### Transaction fees

The transaction fees are calculated based on the number of bytes in the DER encoded transaction.

### Transfers

The most basic type of transaction is a transfer of coins from one wallet to another.

### Trinkets

NFTs and other objects can be minted and exchanged on the blockchain using a trinket transaction. There are some defined trinket formats that allow minting of on-chain or off-chain images as NFTs. Minting an NFT as an on-chain image requires a significant transaction fee depending on the size of the image. Absolutely any unique string of bytes be converted into an NFT.

### Lottery

New coins are exclusively minted through a lottery system. The lottery system is used to regulate how many total fluffycoins exist in the ecosystem. There is no specific maximum or minimum number of coins in the ecosystem, it scales based on the overall volume of transactions on the network. This ensures the more people that use the network, the more coins are available to enable transactions.

Anyone can submit a Lottery Ticket transaction to create a new lottery ticket. The price of the lottery ticket is dynamic, and the more coins spent scales the amount received if the ticket is chosen as a winner but not the chance of winning. Every Reconciliation Block triggers a new lottery drawing with the results based on the entropy of all transactions since the previous Reconciliation Block.

### Loans

Loans on the fluffycoin chain intend to create an incentive to passively stake your coins as public liquidity instead of letting them sit idle. The goal is to provide abundant liquidity to members of the network where the terms are not defined by those with the most capital. Fluffycoin loans work somewhat contraversially in that the risk of the loan is removed from the person lending the capital and placed on the end receiver.

A traditional loan matures over time for the lender who takes on all the risk. Imagine the terms of a loan where $100 is provided to someone today with expectation they will pay $10 a month over the course of 12 months netting the lender 20%. If the person who is lent this money defaults on the loan after the first payment, the lender loses out on $90. The lender takes on all the risk and accordingly gets to choose who is allowed to borrow the money, and what the terms of the agreement will be.

In that traditional loan example, the money is lent all at once to the borrower up front. The borrower can then spend that money immediately and whoever provides goods or services to the borrower ends up with the money in their wallet. In a fluffycoin loan, the borrower is basically issued a digital "IOU" up front. If someone borrows 100 fluffycoins, and then spends it all on groceries, the grocery store ends up holding the "IOU". As the borrower repays the original loan, the "IOU" will automatically convert into fluffycoins over the course of the loan. If the borrower defaults on the loan, the "IOU" will be stuck in that form and stop converting into the remaining owed fluffycoins. This takes all of the risk that used to burden the lender and puts it onto the end receiver of the capital.

This is the part where you say "that's stupid. Who would agree to trade goods or services for money that can disappear? What's stopping the borrower from taking out a loan, buying something, and then immediately defaulting?" But realize this same risk analysis exists in our traditional load example. The lender chooses who's allowed to borrow money and under what terms, just as in a fluffycoin loan the end-receiver of the coins will decide who they accept "IOU"s from and under what terms.

#### Example Loans

McBucks is my best thought experiment to help you imagine a fluffycoin loan. If McDonald's needed to borrow capital, it could take out a fluffycoin loan. They take out a 100 fluffycoin loan that matures in a week, and they receive 100 "IOUs" they call McBucks. They could pay their employees and vendors in McBucks. To McDonald's they get capital up front, and the employees and vendors trust that McDonald's isn't going to default on their loans, so they accept the payment in McBucks knowing in a week it's as good as being paid in fluffycoins. Local businesses that want to entice McDonald's employees and vendors to spend money at their store will offer to accept McBucks, possibly at some small percent markup.

Another example I like to imagine is supporting small businesses. If there is a small business that you believe in, you might accept payment in the "IOU" form as a method of supporting them. You could imagine a small business gaining support from other local businesses by getting them to accept their "IOU" sub-coin, and recruiting employees who believe in the business enough to be paid in these digital IOUs that will mature over time or can be spent immediately at supporting businesses.

### Social Votes

There exists a system on the chain where you can put a topic up for vote. Anyone who wants to pay the fee to submit a vote can put the power of their fluffycoins behind their opinion. Basically staking your coins for your vote.

### Stocks

Stock tickers can be created on the chain and shares of the stock traded. Stocks can be created as privately owned stocks, where all of the shares belong to the origin wallet and they distribute privately as they see fit. Or stocks can be created as public stocks with an IPO price and an IPO duration.

Public stocks have a built in way of providing dividens. A special quirk makes it so they can never sell below the IPO price. If the IPO price is 1 fluffycoin, the stock can only be traded for more than 1 fluffycoin. If it is resold at the IPO price, the stock immediately resolves into that many fluffycoins for the seller and that many fluffycoins is added to a pool specific to that stock. This pool also grows for every trade of that stock. A percentage of the transaction fee from trading a public stock goes into the pool. During Reconciliation Blocks, public stocks traded during the last period have a chance of minting new shares. This will remove the IPO price from the pool for each minted stock, and assign it to a random stock holder.


#### Covered Calls

If you own 100 shares of a stock, you can write a covered call. This creates a call option that can be traded to other people. Whoever owns the call, before it expires, is allowed to execute the call at the strike price defined by the call (The owner of the call option automatically buys the 100 stocks at the strike price). The 100 stocks cannot be traded once they are tied to a call until that call is expired or executed.

Want to add more complicated stock option features, but need to work out the mechanics and there's already so much other stuff to do.

## The blockchain

The binary representation of the blockchain uses DER encoded ASN.1 structures. You can see the format of these structures in the doc/asn1 directory. The initial block is called the Genesis block. It mints the first fluffycoins that need to be used to bootstrap the proof of stake algorithm, and everything verifies back to this block.

#### Shards & Reconciliation Blocks

The blockchain consists of several sub-chains called shards. The algorithm defines which transactions and which validators get assigned to each shard. This is done to increase the throughput of the blockchain. Approximately every 10 minutes all of the shards come back together into a Reconciliation Block. The reconciliation blocks provide some key features.

* Gives a chance to dynamically change the number of shards based on current network volume
* Provides a deliniation between transactions that time-based events on the chain can reference
* Provides a deliniation when a new protocol version becomes adopted and acts as the official validator vote
* Creates a checkpoint where snapshots can be created to speed up validator onboarding (rollup)
* Generates a stronger consensus that all of the shards are coherent with each other

### Algorithms

ECDSA signatures on the ED25519 curve are used for all authentication on the chain. Wallet keys are required to automatically rotate in order to prevent post-quantum attacks on known public key values. A mix of SHA-2 and SHA-3 are used to protect against potential future hash collision techniques. EX25519 and HMAC are used to provide authentication between validator peers.

#### Addresses

Addresses are not only the hash of the public key, but also a user-defined salt value that is prepended to the address. This allows easy personalization of wallet addresses.

## Building

Everything builds and runs in containers. The bash build scripts use docker. So to build this project you only need bash and docker. There are shortcuts to the build scripts in the unix makefile, so the most convenient way to build is with make.

```
$ make help
build-containers               Build the runtime containers
clean                          Cleanup all previous build artifacts and distributables
debug                          Build all debug distributables
debug-docker                   Build all debug distributables using docker - Default
release                        Build all release distributables
release-docker                 Build all release distributables using docker
runcli-docker                  Drop into the CLI docker
runtests-docker                Run unit/integration tests in docker environment
```

### Build code

Make sure you have the submodules checked out before building.
```
git submodule update --init --recursive
```

You can build the binaries using the docker build environment (Recommended).
```
make release-docker
```

Or if you want to build the binaries for the host platform (See dependencies).
```
make release
```

### Build tools and validator services

Once the code is built, the runtime docker images can be built.

```
make build-containers
```

### Use Fluffycoin

You can use the CLI applications to submit transactions and perform other network actions. For convenience there is a CLI docker that can be run and a Makefile command to start it.
```
make build-containers
make runcli-docker
```
```
Fluffycoin Command Line Tools

fluffycoin-genesis: Create a new genesis block
fluffycoin-wallet: Create and manage wallet keys
fluffycoin-info: Query information from the validators
fluffycoin-download: Download all or part of the binary blockchain
fluffycoin-validator: Sign up as a validator, and other validator actions
fluffycoin-transfer: Submit a transfer transaction
fluffycoin-trinket: Submit a trinket transaction
fluffycoin-lottery: Buy a lottery ticket
fluffycoin-loans: Submit actions related to loans
fluffycoin-vote: Submit actions related to votes
fluffycoin-stocks: Submit actions related to stocks
```

### Run validator

Once you have built the runtime containers, the easiest way to run the validator is with docker-compose.

You will need to edit the env file to embed your validator key. When the alpha version is live I will provide a script and an endpoint on fluffyco.in to get free coins to stake.
```
cd docker/validator
cat .env
docker compose up -d
```

### Run tests

You can build and run the unit/integration tests in a docker environment.
```
make debug-docker
make build-containers
make runtests-docker

# Run specific test
make runtests-docker ARGS="-t ApiPort"

# Run test in GDB
make runtests-docker ARGS="-g -t ApiPort"
```

## Dependencies

Everything in this project builds and runs inside of docker containers. To make use of the project you only need docker, bash, and Unix makefiles for the convenient aliases.

Below is a comprehensive list of the dependencies used by the project that are all handled by the Dockerfiles for you.

* OpenSSL - Provides the underlying cryptographic algorithms and ASN.1 encoding
* libzmq - 0MQ networking stack for IPC and P2P communication
* libsodium - Required by libzmq for CurveMQ
* boost ASIO - Asynchronous event handling
* Google protobuf - Performs de/serialization of IPC and P2P messages
* Google gtest - Unit and integration tests
* PostgreSQL - Database for storing current blockchain state
* ozo - Asynchronous interface wrapper for psql
* nlohmann JSON for Modern C++ - JSON formatter for logs
* {fmt} - String formatting

### Build Dependencies

* CMake
* g++ (C++20)
* Ninja
* bash
* patch

#### Build Dependencies of Submodules

* autoconf
* automake
* libtool

### Docker Dependencies

* alpine:edge
* postgres:15-alpine3.18
