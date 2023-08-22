#pragma once

#include <fluffycoin/db/priv/Ozo.h>

namespace fluffycoin::db::priv
{

struct SessionImpl
{
    enum Mode {
        None = 0,
        Connection,     /* <! No work transaction */
        Transaction,    /* <! Work transaction */
    };

    Mode mode = Mode::None;
    Ozo::Connection connection;
    Ozo::Transaction transaction;
};

}
