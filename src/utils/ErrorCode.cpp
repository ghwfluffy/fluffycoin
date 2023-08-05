#include <fluffycoin/utils/ErrorCode.h>

std::string fluffycoin::to_string(ErrorCode code)
{
    std::string ret;

    switch (code)
    {
        case ErrorCode::None: return "None";
        default:
        case ErrorCode::Unknown: return "Unknown";
        case ErrorCode::InternalError: return "Internal Error";
        case ErrorCode::ArgumentMissing: return "Argument Missing";
        case ErrorCode::ArgumentParseError: return "Argument Parse Error";
        case ErrorCode::ArgumentInvalid: return "Argument Invalid";
        case ErrorCode::UnknownCommand: return "Unknown Command";
        case ErrorCode::InvalidMessageFormat: return "Invalid Message Format";
        case ErrorCode::NotLoggedIn: return "Not Logged In";
        case ErrorCode::NotAuthorized: return "Not Authorized";
        case ErrorCode::NoPermission: return "No Permission";
        case ErrorCode::Blocked: return "Blocked";
        case ErrorCode::NotConnected: return "Not Connected";
        case ErrorCode::ResolveError: return "Resolve Error";
        case ErrorCode::ConnectError: return "Connect Error";
        case ErrorCode::ConnectionError: return "Connection Error";
        case ErrorCode::HandshakeError: return "Handshake Error";
        case ErrorCode::ReadError: return "Read Error";
        case ErrorCode::WriteError: return "Write Error";
        case ErrorCode::ProtocolError: return "Protocol Error";
        case ErrorCode::DatabaseError: return "Database Error";
        case ErrorCode::FileError: return "File Error";
        case ErrorCode::DeviceError: return "Device Error";
        case ErrorCode::SocketError: return "Socket Error";
        case ErrorCode::NotFound: return "Not Found";
        case ErrorCode::HardwareError: return "Hardware Error";
        case ErrorCode::MemoryError: return "Memory Error";
        case ErrorCode::IpcTimeout: return "Ipc Timeout";
        case ErrorCode::ServiceTimeout: return "Service Timeout";
        case ErrorCode::SocketTimeout: return "Socket Timeout";
    }

    return ret;
}
