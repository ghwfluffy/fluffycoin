#include <fluffycoin/utils/Details.h>

using namespace fluffycoin;

Details::Details()
{
    logCat = 0;
}

Details::Details(log::Level level)
{
    logCat = 0;
    errorStatus.setLogLevel(level);
}

bool Details::isOk() const
{
    return errorStatus.getStr().empty();
}

ErrorStatus &Details::error()
{
    return errorStatus;
}

const ErrorStatus &Details::error() const
{
    return errorStatus;
}

void Details::setErrorStatus(ErrorStatus errorStatus)
{
    this->errorStatus = std::move(errorStatus);
}
