#ifndef MATCHING_ENGINE_MESSAGES_H
#define MATCHING_ENGINE_MESSAGES_H

#include <cstdint>
#include <string>
#include <string_view>
#include <optional>
#include <variant>

namespace mukhi::matching_engine {

enum class MessageType : uint8_t {
    kAddOrderRequest = 0,
    kCancelOrderRequest = 1,
    kTradeEvent = 2,
    kOrderFullyFilled = 3,
    kOrderPartiallyFilled = 4,
    kUndefined = 10,
};

enum class Side : uint8_t {
    kBuy = 0,
    kSell = 1,
    kUndefined = 10,
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

using InputMessage = std::variant<AddOrderRequest, CancelOrderRequest>;

// Parses one input message, return value is `std::nullopt` if message is ill-formed.
// Format is either of the following:
//
// * msgtype,orderid,side,quantity,price (e.g., 0,123,0,9,1000)
// * msgtype,orderid (e.g., 1,123)
//
// Note that no whitespace is allowed between token and delimter(comma).
std::optional<InputMessage> parse(std::string_view input);

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