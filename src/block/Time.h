#pragma once

#include <chrono>
#include <stdint.h>

#include <openssl/asn1.h>

namespace fluffycoin
{

namespace asn1
{
    typedef struct Time_st Time;
    DECLARE_ASN1_FUNCTIONS(Time)
}

namespace block
{

/**
 * Time in milliseconds since fluffy epoch
 */
class Time
{
    public:
        Time();
        Time(Time &&) = default;
        Time(const Time &) = default;
        Time &operator=(Time &&) = default;
        Time &operator=(const Time &) = default;
        ~Time() = default;

        uint64_t get() const;
        std::chrono::time_point<std::chrono::system_clock> getSystemClock() const;

        void setNow();
        void set(uint64_t time);
        void set(std::chrono::time_point<std::chrono::system_clock> time);

        static Time now();

        void toAsn1(ASN1_INTEGER &) const;
        void fromAsn1(const ASN1_INTEGER &);

        bool operator<(const Time &rhs) const;
        bool operator<=(const Time &rhs) const;
        bool operator>(const Time &rhs) const;
        bool operator>=(const Time &rhs) const;
        bool operator==(const Time &rhs) const;
        bool operator!=(const Time &rhs) const;

    private:
        uint64_t sinceEpoch;
};

}

}
