#pragma once

#include <fluffycoin/Log/log.h>

#include <fluffycoin/svc/ServiceStatusCode.h>

namespace fluffycoin::svc
{

/**
 * Entrypoint for a service
 *
 * @params argc Command line parameter count
 * @params argv Command line parameters
 *
 * @typename ServiceParams The IServiceParams implementation for the service
 */
template<typename ServiceParams>
int run(int argc, const char **argv)
{
    log::info("Service execution starting.");

    // Run until told to shut down
    ServiceStatusCode code = ServiceStatusCode::Running;
    while (code == ServiceStatusCode::Running || code == ServiceStatusCode::Paused)
    {
        code = Service::run<ServiceParams>(argc, argv, code == ServiceStatusCode::Paused);
        log::info("Service execution loop status set to '{}'.", to_string(code));

        if (code == ServiceStatusCode::Restart)
            code = ServiceStatusCode::Running;
        else if (code == ServiceStatusCode::RestartPaused)
            code = ServiceStatusCode::Paused;
    }

    // Convert status code to exit code
    int ret = 0;
    if (code != ServiceStatusCode::Shutdown)
        ret = static_cast<int>(code);

    log::info("Service execution ending with {}.", ret);

    return ret;
}

}
