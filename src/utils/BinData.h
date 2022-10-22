#pragma once

#include <string>

namespace fluffycoin
{

/**
 * Holds binary data
 */
class BinData
{
    public:
        BinData();
        BinData(const unsigned char *data, size_t len);
        BinData(const std::string &str);
        BinData(const char *psz);
        BinData(BinData &&);
        BinData(const BinData &);
        BinData &operator=(BinData &&);
        BinData &operator=(const BinData &);
        ~BinData();

        void clear();
        void resize(size_t len);
        void setData(const unsigned char *data, size_t len);
        void setBuffer(unsigned char *data, size_t len);

        BinData sub(size_t pos, size_t len) const;
        void append(const unsigned char *data, size_t len);
        void prepend(const unsigned char *data, size_t len);

        size_t length() const;
        unsigned char *data();
        const unsigned char *data() const;

        bool empty() const;

        BinData &operator+=(const BinData &data);
        bool operator==(const BinData &rhs) const;

    protected:
        BinData(bool bSecure);

        static void zeroize(void *pv, size_t len);

    private:
        void cleanup();

        bool bSecure;
        unsigned char *buffer;
        size_t len;
};

}
