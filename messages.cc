#include "messages.h"

#include <format>
#include <string>

namespace mukhi::matching_engine
{

    std::string TradeEvent::to_string()
    {
        return std::format("{},{},{}",
                           static_cast<uint8_t>(MessageType::kTradeEvent), qty, price);
    }

    std::string OrderFullyFilled::to_string()
    {
        return std::format("{},{}", static_cast<uint8_t>(MessageType::kOrderFullyFilled), order_id);
    }

    std::string OrderPartiallyFilled::to_string()
    {
        return std::format("{},{},{}", static_cast<uint8_t>(MessageType::kOrderPartiallyFilled), order_id, remaining);
    }

} // mmukhi::matching_engine