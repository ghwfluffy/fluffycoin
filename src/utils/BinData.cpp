#include <fluffycoin/utils/BinData.h>
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
            unsigned char *newData = reinterpret_cast<unsigned char *>(malloc(len));
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
