#ifndef MATCHING_ENGINE_MESSAGES_H
#define MATCHING_ENGINE_MESSAGES_H

#include <cstdint>
#include <string>

namespace mukhi::matching_engine {

enum class MessageType : uint8_t {
    kAddOrderRequest = 0,
    kCancelOrderRequest = 1,
    kTradeEvent = 2,
    kOrderFullyFilled = 3,
    kOrderPartiallyFilled = 4,
};

enum class Side : uint8_t {
    kBuy = 0,
    kSell = 1,
};

using OrderId = uint64_t;
using Quantity = uint64_t;
using Price = double;

// Input messages.

struct AddOrderRequest {
    OrderId order_id;
    Side side;
    Quantity qty;
    Price price;
};

struct CancelOrderRequest {
    OrderId order_id;
};

// Output messages.

struct TradeEvent {
    Quantity qty;
    Price price;
};

std::ostream& operator<<(std::ostream& os, const TradeEvent& obj);

struct OrderFullyFilled {
    OrderId order_id;
};

std::ostream& operator<<(std::ostream& os, const OrderFullyFilled& obj);

struct OrderPartiallyFilled {
    OrderId order_id;
    Quantity remaining;
};

std::ostream& operator<<(std::ostream& os, const OrderPartiallyFilled& obj);

} // namespace mukhi::matching_engine

#endif // MATCHING_ENGINE_MESSAGES_H