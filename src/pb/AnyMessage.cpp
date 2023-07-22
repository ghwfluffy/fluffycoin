#include <fluffycoin/pb/AnyMessage.h>
#include <fluffycoin/pb/Catalog.h>

using namespace fluffycoin::pb;

void AnyMessage::fromAny(
    const google::protobuf::Any &input,
    std::unique_ptr<google::protobuf::Message> &output,
    Details &details)
{
    // Get type
    const std::string &type = input.type_url();

    // Create new of that type
    output = Catalog::newMsg(type);
    if (!output)
        details.setError(ErrorCode::InternalError, "from_any", "Unknown message type '{}'.", type);

    // Deserialize from Any
    if (details.isOk() && !input.UnpackTo(output.get()))
        details.setError(ErrorCode::InternalError, "from_any", "Failed to deserialize any '{}'.", type);
}

void AnyMessage::toAny(
    const google::protobuf::Message &input,
    google::protobuf::Any &output)
{
    output.PackFrom(input);
}
