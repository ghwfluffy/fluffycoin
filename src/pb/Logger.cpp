#include <fluffycoin/pb/Logger.h>

#include <fluffycoin/log/Log.h>
#include <fluffycoin/log/Category.h>

#include <absl/log/log_sink.h>
#include <absl/log/log_sink_registry.h>

using namespace fluffycoin;
using namespace fluffycoin::pb;

namespace
{

enum ProtobufCategory
{
    Protobuf
};

bool bInit = false;
/**
 * Intercepts Abseil logs and forwards to fluffycoin logger
 */
class : public absl::LogSink
{
    void Send(const absl::LogEntry &entry) final
    {
        log::Level eLevel = log::Level::Traffic;
        switch (entry.log_severity())
        {
            default:
            case absl::LogSeverity::kInfo:
                eLevel = log::Level::Debug;
                break;
            case absl::LogSeverity::kWarning:
                eLevel = log::Level::Warn;
                break;
            case absl::LogSeverity::kError:
            case absl::LogSeverity::kFatal:
                eLevel = log::Level::Error;
                break;
        }

        absl::string_view text = entry.text_message_with_prefix();
        log::msg(eLevel, std::string(text.data(), text.length()));
    }
} logger;

}

void Logger::init()
{
    if (!bInit)
    {
        bInit = true;
        log::Category::add(Protobuf, "PROTOBUF");
        absl::AddLogSink(&logger);
    }
}
