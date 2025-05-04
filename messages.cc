#include "messages.h"

#include <string>
#include <iostream>

namespace mukhi::matching_engine {

namespace {
uint32_t to_num(const MessageType& t) {
    return static_cast<uint32_t>(t);
}
}  // namespace

std::ostream& operator<<(std::ostream& os, const TradeEvent& obj) {
    os << to_num(MessageType::kTradeEvent) << "," << obj.qty << "," << obj.price;
    return os;
}

std::ostream& operator<<(std::ostream& os, const OrderFullyFilled& obj) {
    os << to_num(MessageType::kOrderFullyFilled) << "," << obj.order_id;
    return os;
}

std::ostream& operator<<(std::ostream& os, const OrderPartiallyFilled& obj) {
    os << to_num(MessageType::kOrderPartiallyFilled) << "," << obj.order_id << "," << obj.remaining;
    return os;
}

} // mmukhi::matching_engine