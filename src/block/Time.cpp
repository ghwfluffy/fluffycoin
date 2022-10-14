#include <fluffycoin/block/Time.h>
#include <fluffycoin/ossl/convert.h>

using namespace fluffycoin;
using namespace fluffycoin::block;

namespace
{

constexpr const int64_t fluffyEpochDelta = 0xD87B28B000LL;

}

Time::Time()
{
    sinceEpoch = 0;
}

uint64_t Time::get() const
{
    return sinceEpoch;
}

std::chrono::time_point<std::chrono::system_clock> Time::getSystemClock() const
{
    return std::chrono::system_clock::from_time_t(
        static_cast<std::time_t>(sinceEpoch) + static_cast<std::time_t>(fluffyEpochDelta));
}

void Time::set(uint64_t time)
{
    this->sinceEpoch = time;
}

void Time::set(std::chrono::time_point<std::chrono::system_clock> time)
{
    // Convert to integer
    int64_t sinceLinuxEpoch =
        static_cast<int64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
            time.time_since_epoch()).count());

    // Convert to fluffy epoch
    if (sinceLinuxEpoch < fluffyEpochDelta)
        sinceEpoch = 0;
    else
        sinceEpoch = static_cast<uint64_t>(sinceLinuxEpoch - fluffyEpochDelta);
}

void Time::setNow()
{
    set(std::chrono::system_clock::now());
}

void Time::toAsn1(ASN1_INTEGER &t) const
{
    ossl::fromUInt64(t, sinceEpoch);
}

void Time::fromAsn1(const ASN1_INTEGER &t)
{
    sinceEpoch = ossl::toUInt64(t);
}

bool Time::operator<(const Time &rhs) const
{
    return sinceEpoch < rhs.sinceEpoch;
}

bool Time::operator<=(const Time &rhs) const
{
    return sinceEpoch <= rhs.sinceEpoch;
}

bool Time::operator>(const Time &rhs) const
{
    return sinceEpoch > rhs.sinceEpoch;
}

bool Time::operator>=(const Time &rhs) const
{
    return sinceEpoch >= rhs.sinceEpoch;
}

bool Time::operator==(const Time &rhs) const
{
    return sinceEpoch == rhs.sinceEpoch;
}

bool Time::operator!=(const Time &rhs) const
{
    return sinceEpoch != rhs.sinceEpoch;
}
