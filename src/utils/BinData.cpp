#include <fluffycoin/utils/BinData.h>

#include <fluffycoin/log/Log.h>

#include <string.h>

using namespace fluffycoin;

namespace
{

typedef void *(*memset_sig)(void *, int, size_t);
volatile memset_sig volatile_memset = memset;

}

BinData::BinData()
{
    len = 0;
    buffer = nullptr;
    bSecure = false;
}

BinData::BinData(bool bSecure)
        : BinData()
{
    this->bSecure = bSecure;
}

BinData::BinData(const unsigned char *buffer, size_t len)
        : BinData()
{
    setData(buffer, len);
}

BinData::BinData(const char *psz)
        : BinData()
{
    if (psz && *psz)
        setData(reinterpret_cast<const unsigned char *>(psz), strlen(psz));
}

BinData::BinData(const std::string &str)
        : BinData()
{
    setData(reinterpret_cast<const unsigned char *>(str.c_str()), str.length());
}

BinData::BinData(BinData &&rhs)
        : BinData()
{
    operator=(std::move(rhs));
}

BinData::BinData(const BinData &rhs)
        : BinData()
{
    operator=(rhs);
}

BinData &BinData::operator=(BinData &&rhs)
{
    if (this != &rhs)
    {
        cleanup();

        this->bSecure = rhs.bSecure;
        this->buffer = rhs.buffer;
        this->len = rhs.len;

        rhs.buffer = nullptr;
        rhs.len = 0;
    }

    return (*this);
}

BinData &BinData::operator=(const BinData &rhs)
{
    if (this != &rhs)
    {
        setData(rhs.buffer, rhs.len);
        this->bSecure = rhs.bSecure;
    }

    return (*this);
}

BinData::~BinData()
{
    cleanup();
}

void BinData::cleanup()
{
    if (bSecure && this->buffer && this->len > 0)
        zeroize(this->buffer, this->len);
    free(this->buffer);
    this->len = 0;
    this->buffer = nullptr;
}

void BinData::clear()
{
    cleanup();
}

void BinData::resize(size_t len)
{
    if (this->len != len)
    {
        if (!buffer)
        {
            this->buffer = reinterpret_cast<unsigned char *>(calloc(1, len));
            memset(this->buffer, 0, len);
            this->len = len;
        }
        else if (!bSecure)
        {
            this->buffer = reinterpret_cast<unsigned char *>(realloc(this->buffer, len));
            if (len > this->len)
                memset(this->buffer + this->len, 0, len - this->len);
            this->len = len;
        }
        else
        {
            unsigned char *newData = reinterpret_cast<unsigned char *>(calloc(1, len));
            size_t minLen = len < this->len ? len : this->len;
            memcpy(newData, buffer, minLen);
            if (len > this->len)
                zeroize(newData + this->len, len - this->len);
            cleanup();
            this->buffer = newData;
            this->len = len;
        }
    }
}

void BinData::setData(const unsigned char *buffer, size_t len)
{
    // Free old buffer
    cleanup();

    // Set new buffer
    if (buffer && len > 0)
    {
        this->buffer = reinterpret_cast<unsigned char *>(malloc(len));
        memcpy(this->buffer, buffer, len);
        this->len = len;
    }
}

void BinData::setBuffer(unsigned char *data, size_t len)
{
    // Free old buffer
    cleanup();
    if (!len)
        free(data);
    else if (!data)
        log::error("{} length for null buffer provided.", len);
    else
    {
        this->buffer = data;
        this->len = len;
    }
}

void BinData::truncateFront(size_t bytes)
{
    if (bytes > this->len)
        bytes = this->len;

    if (bytes > 0)
    {
        memmove(this->buffer, this->buffer + bytes, this->len - bytes);
        this->len -= bytes;
    }
}

BinData BinData::sub(size_t pos, size_t len) const
{
    BinData ret(bSecure);
    if (len == 0)
        return ret;

    if (pos > this->len)
    {
        log::error("Attempted to retrieve data from position {} of {} length data.", pos, this->len);
        pos = 0;
        len = 0;
    }
    else if (pos + len > this->len)
    {
        log::error("Attempted to retrieve {} bytes from position {} of {} length data.", len, pos, this->len);
        len = this->len - pos;
    }

    ret.setData(this->buffer + pos, len);
    return ret;
}

void BinData::append(const unsigned char *data, size_t len)
{
    if (len == 0)
        return;
    if (!data)
    {
        log::error("Attempting to append null data.");
        return;
    }

    size_t newLen = this->len + len;
    unsigned char *newBuffer = reinterpret_cast<unsigned char *>(malloc(newLen));

    memcpy(newBuffer, buffer, this->len);
    memcpy(newBuffer + this->len, data, len);

    cleanup();
    this->buffer = newBuffer;
    this->len = newLen;
}

void BinData::prepend(const unsigned char *data, size_t len)
{
    if (len == 0)
        return;
    if (!data)
    {
        log::error("Attempting to prepend null data.");
        return;
    }

    size_t newLen = this->len + len;
    unsigned char *newBuffer = reinterpret_cast<unsigned char *>(malloc(newLen));

    memcpy(newBuffer, data, len);
    memcpy(newBuffer + len, buffer, this->len);

    cleanup();
    this->buffer = newBuffer;
    this->len = newLen;
}

size_t BinData::length() const
{
    return len;
}

unsigned char *BinData::data()
{
    return buffer;
}

const unsigned char *BinData::data() const
{
    return buffer;
}

void BinData::zeroize(void *pv, size_t len)
{
    if (pv && len)
        volatile_memset(pv, 0, len);
}

bool BinData::empty() const
{
    return len == 0;
}

BinData &BinData::operator+=(const BinData &data)
{
    append(data.data(), data.length());
    return (*this);
}

bool BinData::operator==(const BinData &rhs) const
{
    return length() == rhs.length() &&
            memcmp(data(), rhs.data(), length()) == 0;
}

bool BinData::operator!=(const BinData &rhs) const
{
    return !((*this) == rhs);
}

BinData operator+(const BinData &lhs, const BinData &rhs)
{
    BinData ret(lhs);
    ret += rhs;
    return ret;
}
