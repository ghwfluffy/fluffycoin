#include <fluffycoin/db/Result.h>

using namespace fluffycoin;
using namespace fluffycoin::db;

Result::Result()
{
    uiAffectedRows = 0;
}

size_t Result::getAffectedRows() const
{
    return uiAffectedRows;
}

void Result::setAffectedRows(size_t ui)
{
    this->uiAffectedRows = ui;
}
