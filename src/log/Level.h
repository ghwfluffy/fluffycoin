#pragma once

#include <string>

namespace fluffycoin
{

namespace log
{
    /**
     * Levels for log verbosity
     */
    enum class Level
    {
        None = 0,       /* <! Do not log */
        Error,          /* <! Things that should not happen and are triggering an error state */
        Warn,           /* <! Things that should not happen but are not triggering an error state */
        Notice,         /* <! Unusual events */
        Info,           /* <! Events that are useful to know about */
        Debug,          /* <! Information useful for debugging a module */
        Traffic         /* <! Very verbose logs such as trace logs */
    };
}

std::string to_string(log::Level);
void from_string(log::Level &, const std::string &);

}
