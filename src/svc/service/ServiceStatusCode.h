#pragma once

#include <string>

namespace fluffycoin::svc
{

/**
 * Status code returned when a service has decided to stop processing
 */
enum class ServiceStatusCode : int
{
    None = 0,
    Running,
    Paused,
    Restart,
    RestartPaused,
    Shutdown,
};

std::string to_string(ServiceStatusCode);

}
