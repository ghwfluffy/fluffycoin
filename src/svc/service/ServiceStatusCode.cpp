#include <fluffycoin/svc/ServiceStatusCode.h>
#include <fluffycoin/utils/Strings.h>

namespace fluffycoin::svc
{

std::string to_string(ServiceStatusCode code)
{
    switch (code)
    {
        default:
        case ServiceStatusCode::None:
            return "Unknown " + fluffycoin::to_string(static_cast<int>(code));
        case ServiceStatusCode::Running:
            return "Running";
        case ServiceStatusCode::Paused:
            return "Paused";
        case ServiceStatusCode::Restart:
            return "Restart";
        case ServiceStatusCode::RestartPaused:
            return "Restart Paused";
        case ServiceStatusCode::Shutdown:
            return "Shutdown";
    }
}

}
