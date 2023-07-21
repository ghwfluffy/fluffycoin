#pragma once

#include <string>

namespace fluffycoin
{

/**
 * Error codes to allow programatically handling
 * error results
 */
enum class ErrorCode
{
    None = 0,
    Unknown = 1,
    InternalError = 2,

    // Application layer protocol
    ArgumentMissing = 100,
    ArgumentParseError = 101,
    ArgumentInvalid = 102,
    UnknownCommand = 103,
    InvalidMessageFormat = 104,

    // Auth errors
    /*Reserved = 200,*/
    NotLoggedIn = 201,
    NotAuthorized = 202,
    NoPermission = 203,

    // Communication errors
    NotConnected = 300,
    ResolveError = 301,
    ConnectError = 302,
    ConnectionError = 303,
    HandshakeError = 304,
    ReadError = 305,
    WriteError = 306,
    ProtocolError = 307,

    // Resource errors
    DatabaseError = 400,
    FileError = 401,
    DeviceError = 402,
    SocketError = 403,
    NotFound = 404,
    HardwareError = 405,
    MemoryError = 406,

    // Timeouts
    IpcTimeout = 500,
    ServiceTimeout = 501,
    SocketTimeout = 502,
};

std::string to_string(ErrorCode);

}
