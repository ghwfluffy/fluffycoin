#include <fluffycoin/utils/SafeData.h>

using namespace fluffycoin;

SafeData::SafeData()
        : BinData(true)
{
}

SafeData::SafeData(const std::string &data)
        : BinData(true)
{
    setData(reinterpret_cast<const unsigned char *>(data.c_str()), data.length());
}

SafeData::SafeData(SafeData &&rhs)
        : BinData(true)
{
    operator=(std::move(rhs));
}

SafeData::SafeData(const SafeData &rhs)
        : BinData(true)
{
    operator=(rhs);
}

SafeData &SafeData::operator=(SafeData &&rhs)
{
    BinData::operator=(std::move(rhs));
    return (*this);
}

SafeData &SafeData::operator=(const SafeData &rhs)
{
    BinData::operator=(rhs);
    return (*this);
}

SafeData::SafeData(BinData &&rhs)
        : BinData(true)
{
    BinData::operator=(std::move(rhs));
}

SafeData::SafeData(const BinData &rhs)
        : BinData(true)
{
    BinData::operator=(rhs);
}

SafeData &SafeData::operator=(BinData &&rhs)
{
    BinData::operator=(std::move(rhs));
    return (*this);
}

SafeData &SafeData::operator=(const BinData &rhs)
{
    BinData::operator=(rhs);
    return (*this);
}
