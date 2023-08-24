#include <fluffycoin/db/DataResult.h>

using namespace fluffycoin;
using namespace fluffycoin::db;

namespace
{

class : public IResultRow
{
    public:
        operator bool() const
        {
            return false;
        }

        const IResultCol &operator[](size_t pos) const final
        {
            (void)pos;
            return col;
        }

    private:
        class : public IResultCol
        {
            public:
                BinData data() const final { return BinData(); }
                std::string str() const final { return std::string(); }
                uint32_t UInt32() const final { return uint32_t(); }
                uint64_t UInt64() const final { return uint64_t(); }
                int32_t Int32() const final { return int32_t(); }
                int64_t Int64() const final { return int64_t(); }
                bool boolean() const final { return false; }
                bool null() const final { return true; }
        } col;
} nullRow;

}

DataResult::DataResult(std::unique_ptr<IResultData> data)
{
    this->data = std::move(data);
}

size_t DataResult::getNumResults() const
{
    return data ? data->getNumResults() : 0;
}

const IResultRow &DataResult::cur() const
{
    return data ? data->cur() : nullRow;
}

const IResultRow &DataResult::next() const
{
    return data ? data->cur() : nullRow;
}
