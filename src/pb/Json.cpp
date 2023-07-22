#include <fluffycoin/pb/Json.h>

#include <google/protobuf/util/json_util.h>

using namespace fluffycoin;
using namespace fluffycoin::pb;

void Json::fromJson(
    const char *data,
    size_t len,
    google::protobuf::Message &msg,
    Details &details)
{
    google::protobuf::json::ParseOptions options;
    options.ignore_unknown_fields = true;
    options.case_insensitive_enum_parsing = true;

    absl::Status status =
        google::protobuf::util::JsonStringToMessage(
            absl::string_view(data, len),
            &msg,
            options);
    if (status.code() != absl::StatusCode::kOk)
    {
        details.setError(ErrorCode::InvalidMessageFormat, "json",
            "Failed to parse message: {}", status.message().data());
    }
}

void Json::toJson(
    const google::protobuf::Message &msg,
    std::string &data,
    Details &details)
{
    google::protobuf::json::PrintOptions options;
    options.add_whitespace = false;
    options.always_print_primitive_fields = true;
    options.always_print_enums_as_ints = false;
    options.preserve_proto_field_names = true;
    options.unquote_int64_if_possible = false;

    absl::Status status =
        google::protobuf::util::MessageToJsonString(
            msg,
            &data,
            options);
    if (status.code() != absl::StatusCode::kOk)
    {
        details.setError(ErrorCode::InternalError, "json",
            "Failed to serialize message: {}", status.message().data());
    }
    else if (data.empty())
    {
        details.setError(ErrorCode::InternalError, "json",
            "Serialized empty JSON message.");
    }
}
