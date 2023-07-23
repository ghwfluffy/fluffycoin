#include <fluffycoin/svc/EventSubscriptionMap.h>

using namespace fluffycoin;
using namespace fluffycoin::svc;

const EventSubscriptionMap::EventHandler *EventSubscriptionMap::getServiceHandler(
    uint16_t port,
    const std::string &topic) const
{
    // Find event port
    auto iter = mapServiceHandlers.find(port);
    if (iter == mapServiceHandlers.end())
        return nullptr;

    // Find topic
    const std::map<std::string, EventHandler> &handlers = iter->second;
    auto iterHandler = handlers.find(topic);
    // Find empty topic (all topics)
    if (iterHandler == handlers.end())
        iterHandler = handlers.find(std::string());

    return iterHandler == handlers.end() ? nullptr : &iterHandler->second;
}

const EventSubscriptionMap::EventHandler *EventSubscriptionMap::getPeerHandler(
    const std::string &topic) const
{
    auto iter = mapPeerHandlers.find(topic);
    return iter == mapPeerHandlers.end() ? nullptr : &iter->second;
}

void EventSubscriptionMap::addHandler(
    uint16_t port,
    const std::string &topic,
    EventHandler handler)
{
    mapServiceHandlers[port][topic] = std::move(handler);
}

void EventSubscriptionMap::addPeerHandler(
    const std::string &topic,
    EventHandler handler)
{
    mapPeerHandlers[topic] = std::move(handler);
}

std::list<std::string> EventSubscriptionMap::getPeerSubscriptions() const
{
    std::list<std::string> topics;
    for (const auto &pair : mapPeerHandlers)
        topics.push_back(pair.first);
    return topics;
}

std::map<uint16_t, std::list<std::string>> EventSubscriptionMap::getServiceSubscriptions() const
{
    std::map<uint16_t, std::list<std::string>> topics;
    for (const auto &portPair : mapServiceHandlers)
    {
        for (const auto &topicPair : portPair.second)
            topics[portPair.first].push_back(topicPair.first);
    }
    return topics;
}
