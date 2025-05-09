#include "messages.h"

#include <charconv>
#include <iostream>
#include <string>
#include <string_view>

namespace mukhi::matching_engine {

namespace {
constexpr uint32_t kErrLimit = 50;

uint32_t to_num(const MessageType& t) { return static_cast<uint32_t>(t); }
MessageType to_msg_type(uint32_t t) {
  switch (t) {
    case 0:
      return MessageType::kAddOrderRequest;
    case 1:
      return MessageType::kCancelOrderRequest;
    case 2:
      return MessageType::kTradeEvent;
    case 3:
      return MessageType::kOrderFullyFilled;
    case 4:
      return MessageType::kOrderPartiallyFilled;
    default:
      return MessageType::kUndefined;
  }
}

Side to_side_type(uint8_t t) {
  switch (t) {
    case 0:
      return Side::kBuy;
    case 1:
      return Side::kSell;
    default:
      return Side::kUndefined;
  }
}

std::optional<AddOrderRequest> ParseAddOrderRequest(std::string_view input,
                                                    std::ostream& es) {
  size_t pos = input.find(",");
  if (pos == std::string::npos) {
    es << "Bad Message: Unparsable add order request : "
       << input.substr(0, kErrLimit) << std::endl;
    return std::nullopt;
  }
  AddOrderRequest req;
  auto result = std::from_chars(input.data(), input.data() + pos, req.order_id);
  if (result.ec != std::errc() || result.ptr != input.data() + pos) {
    es << "Bad Message: Unparsable order id in add order request : " << input
       << std::endl;
    return std::nullopt;
  }
  input = input.substr(pos + 1);
  pos = input.find(",");
  if (pos == std::string::npos) {
    es << "Bad Message: Unparsable add order request : "
       << input.substr(0, kErrLimit) << std::endl;
    return std::nullopt;
  }
  uint8_t side;
  result = std::from_chars(input.data(), input.data() + pos, side);
  if (result.ec != std::errc() || result.ptr != input.data() + pos) {
    es << "Bad Message: Unparsable 'side' in add order request : " << input
       << std::endl;
    return std::nullopt;
  }
  if (auto s = to_side_type(side); s != Side::kUndefined) {
    req.side = s;
  } else {
    es << "Bad Message: Unknown value for 'side' in add order request : "
       << input.substr(0, kErrLimit) << std::endl;
    return std::nullopt;
  }
  input = input.substr(pos + 1);
  pos = input.find(",");
  if (pos == std::string::npos) {
    es << "Bad Message: Unparsable add order request : "
       << input.substr(0, kErrLimit) << std::endl;
    return std::nullopt;
  }
  result = std::from_chars(input.data(), input.data() + pos, req.qty);
  if (result.ec != std::errc() || result.ptr != input.data() + pos) {
    es << "Bad Message: Unparsable 'quantity' in add order request: " << input
       << std::endl;
    return std::nullopt;
  }
  input = input.substr(pos + 1);

  // Note that `std::from_chars` isn't available for floating points in libc++
  // standard library used by clang on mac os (dev environment). Therefore, we
  // will rely on `std::stod` for parsing `price`. As such we must check for
  // leading whitespaces, and trailing whitespaces as well as non-numeric
  // characters.
  try {
    if (std::isspace(input.at(0))) {
      es << "Bad Message: Unparsable 'price' in add order request : " << input
         << std::endl;
      return std::nullopt;
    }
    req.price = std::stod(input.data(), &pos);
  } catch (const std::invalid_argument& e) {
    es << "Bad Message: exception while parsing 'price': " << e.what()
       << std::endl;
    return std::nullopt;
  } catch (const std::out_of_range& e) {
    es << "Bad Message: exception while parsing 'price': " << e.what()
       << std::endl;
    return std::nullopt;
  }
  if (input.size() != pos) {
    es << "Bad Message: Unparsable add order request : "
       << input.substr(0, kErrLimit) << std::endl;
    return std::nullopt;
  }
  return req;
}

std::optional<CancelOrderRequest> ParseCancelOrderRequest(
    std::string_view input, std::ostream& es) {
  OrderId order_id;
  auto [ptr, ec] =
      std::from_chars(input.data(), input.data() + input.size(), order_id);
  if (ec != std::errc() || ptr != input.data() + input.size()) {
    es << "Bad message: Unparsable order id in cancel order request : " << input
       << std::endl;
    return std::nullopt;
  }
  return CancelOrderRequest{.order_id = order_id};
}

}  // namespace

std::optional<InputMessage> parse(std::string_view input, std::ostream& es) {
  size_t pos = input.find(",");
  if (pos == std::string::npos) {
    es << "Bad message: Unknown format : " << input.substr(0, kErrLimit)
       << std::endl;
    return std::nullopt;
  }
  uint8_t type{};
  auto [ptr, ec] = std::from_chars(input.data(), input.data() + pos, type);
  if (ec != std::errc() || ptr != input.data() + pos) {
    es << "Bad message: Unknown type : " << input.substr(0, kErrLimit)
       << std::endl;
    return std::nullopt;
  }
  switch (to_msg_type(type)) {
    case MessageType::kAddOrderRequest:
      return ParseAddOrderRequest(input.substr(pos + 1), es);
    case MessageType::kCancelOrderRequest:
      return ParseCancelOrderRequest(input.substr(pos + 1), es);
    default:
      es << "Bad message: Invalid type : " << input.substr(0, kErrLimit)
         << std::endl;
      return std::nullopt;
  }
}

std::ostream& operator<<(std::ostream& os, const TradeEvent& obj) {
  os << to_num(MessageType::kTradeEvent) << "," << obj.qty << "," << obj.price;
  return os;
}

std::ostream& operator<<(std::ostream& os, const OrderFullyFilled& obj) {
  os << to_num(MessageType::kOrderFullyFilled) << "," << obj.order_id;
  return os;
}

std::ostream& operator<<(std::ostream& os, const OrderPartiallyFilled& obj) {
  os << to_num(MessageType::kOrderPartiallyFilled) << "," << obj.order_id << ","
     << obj.remaining;
  return os;
}

}  // namespace mukhi::matching_engine