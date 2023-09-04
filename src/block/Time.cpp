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
    std::chrono::time_point<std::chrono::system_clock> tp =
        std::chrono::system_clock::from_time_t(
            static_cast<std::time_t>(sinceEpoch / 1000LL) + static_cast<std::time_t>(fluffyEpochDelta));
    tp += std::chrono::milliseconds(sinceEpoch % 1000LL);
    return tp;
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

Time Time::now()
{
    Time ret;
    ret.setNow();
    return ret;
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

std::string fluffycoin::to_string(const block::Time &rhs)
{
    // Convert time_point to time_t
    std::chrono::time_point<std::chrono::system_clock> tp(rhs.getSystemClock());
    std::time_t tt = std::chrono::system_clock::to_time_t(tp);
    std::tm *gmt = std::gmtime(&tt);

    // Extract milliseconds
    auto duration = tp.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() % 1000;

    std::stringstream ss;
    ss << std::put_time(gmt, "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << millis;
    return ss.str();
}
