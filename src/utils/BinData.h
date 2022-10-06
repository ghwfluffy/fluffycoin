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
        BinData(BinData &&);
        BinData(const BinData &);
        BinData &operator=(BinData &&);
        BinData &operator=(const BinData &);
        ~BinData();

        void resize(size_t len);
        void setData(const unsigned char *data, size_t len);

        size_t length() const;
        unsigned char *data();
        const unsigned char *data() const;

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
