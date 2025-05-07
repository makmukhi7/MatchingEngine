#include "messages.h"

#include <gtest/gtest.h>
#include <sstream>

namespace mukhi::matching_engine {
TEST(TradeEvent, to_string) {
    TradeEvent te{
        .qty = 10,
        .price = 15.0,
    };

    std::stringstream ss;
    ss << te;
    EXPECT_EQ(ss.str(), "2,10,15");
}

TEST(OrderFullyFilled, to_string) {
    OrderFullyFilled of{ .order_id = 100000 };

    std::stringstream ss;
    ss << of;
    EXPECT_EQ(ss.str(), "3,100000");
}

TEST(OrderPartiallyFilled, to_string) {
    OrderPartiallyFilled opf{ .order_id = 1000001, .remaining = 75 };

    std::stringstream ss;
    ss << opf;
    EXPECT_EQ(ss.str(), "4,1000001,75");
}

TEST(Parse, AddOrderRequestSell) {
    std::string line = "0,1000000,1,45,1075.5";
    std::stringstream ss;
    std::optional<InputMessage> msg = parse(line, ss);
    ASSERT_NE(msg, std::nullopt);
    ASSERT_TRUE(std::holds_alternative<AddOrderRequest>(*msg));
    EXPECT_EQ(std::get<AddOrderRequest>(*msg).order_id, 1000000);
    EXPECT_EQ(std::get<AddOrderRequest>(*msg).side, Side::kSell);
    EXPECT_EQ(std::get<AddOrderRequest>(*msg).qty, 45);
    EXPECT_EQ(std::get<AddOrderRequest>(*msg).price, 1075.5);
}

TEST(Parse, AddOrderRequestBuy) {
    std::string line = "0,1000004,0,4500,1075324.5";
    std::stringstream ss;
    std::optional<InputMessage> msg = parse(line, ss);
    ASSERT_NE(msg, std::nullopt);
    ASSERT_TRUE(std::holds_alternative<AddOrderRequest>(*msg));
    EXPECT_EQ(std::get<AddOrderRequest>(*msg).order_id, 1000004);
    EXPECT_EQ(std::get<AddOrderRequest>(*msg).side, Side::kBuy);
    EXPECT_EQ(std::get<AddOrderRequest>(*msg).qty, 4500);
    EXPECT_EQ(std::get<AddOrderRequest>(*msg).price, 1075324.5);
}

TEST(Parse, CancelOrderRequest) {
    std::string line = "1,1000000";
    std::stringstream ss;
    std::optional<InputMessage> msg = parse(line, ss);
    ASSERT_NE(msg, std::nullopt);
    ASSERT_TRUE(std::holds_alternative<CancelOrderRequest>(*msg));
    EXPECT_EQ(std::get<CancelOrderRequest>(*msg).order_id, 1000000);
}

TEST(Parse, SpacesNotAllowed) {
    std::string line = "0 ,1000000,1,1,1075";
    std::stringstream ss;
    std::optional<InputMessage> msg = parse(line, ss);
    ASSERT_EQ(msg, std::nullopt);
}

TEST(Parse, TypeOutOfRange) {
    std::string line = "100000000,1000000,1,1,1075";
    std::stringstream ss;
    std::optional<InputMessage> msg = parse(line, ss);
    ASSERT_EQ(msg, std::nullopt);
}

TEST(Parse, BadFormat) {
    std::string line = "applesandoranges";
    std::stringstream ss;
    std::optional<InputMessage> msg = parse(line, ss);
    ASSERT_EQ(msg, std::nullopt);
}

TEST(Parse, EmptyString) {
    std::string line = "";
    std::stringstream ss;
    std::optional<InputMessage> msg = parse(line, ss);
    ASSERT_EQ(msg, std::nullopt);
}

TEST(Parse, PartialString) {
    std::string line = "1,";
    std::stringstream ss;
    std::optional<InputMessage> msg = parse(line, ss);
    ASSERT_EQ(msg, std::nullopt);
}

TEST(Parse, BadOrderId) {
    std::string line = "1,asdf";
    std::stringstream ss;
    std::optional<InputMessage> msg = parse(line, ss);
    ASSERT_EQ(msg, std::nullopt);
}

TEST(Parse, BadOrderIdWithTrailingSpace) {
    std::string line = "1,100001 ";
    std::stringstream ss;
    std::optional<InputMessage> msg = parse(line, ss);
    ASSERT_EQ(msg, std::nullopt);
}

TEST(Parse, BadSide) {
    std::string line = "0,1000000,2,1,1075";
    std::stringstream ss;
    std::optional<InputMessage> msg = parse(line, ss);
    ASSERT_EQ(msg, std::nullopt);
}

TEST(Parse, BadSideWithSpace) {
    std::string line = "0,1000000,1 ,1,1075";
    std::stringstream ss;
    std::optional<InputMessage> msg = parse(line, ss);
    ASSERT_EQ(msg, std::nullopt);
}

TEST(Parse, BadQuantity) {
    std::string line = "0,1000000,1,words,1075";
    std::stringstream ss;
    std::optional<InputMessage> msg = parse(line, ss);
    ASSERT_EQ(msg, std::nullopt);
}

TEST(Parse, NegativeQuantity) {
    std::string line = "0,1000000,1,-1,1075";
    std::stringstream ss;
    std::optional<InputMessage> msg = parse(line, ss);
    ASSERT_EQ(msg, std::nullopt);
}

TEST(Parse, BadQuantityWithLeadingSpace) {
    std::string line = "0,1000000,1, 10,1075";
    std::stringstream ss;
    std::optional<InputMessage> msg = parse(line, ss);
    ASSERT_EQ(msg, std::nullopt);
}

TEST(Parse, BadPrice) {
    std::string line = "0,1000000,1,10,word";
    std::stringstream ss;
    std::optional<InputMessage> msg = parse(line, ss);
    ASSERT_EQ(msg, std::nullopt);
}

TEST(Parse, BadPriceTrailingComma) {
    std::string line = "0,1000000,1,10,101.7,";
    std::stringstream ss;
    std::optional<InputMessage> msg = parse(line, ss);
    ASSERT_EQ(msg, std::nullopt);
}

TEST(Parse, BadPriceTrailingSpace) {
    std::string line = "0,1000000,1,10,101.7 ";
    std::stringstream ss;
    std::optional<InputMessage> msg = parse(line, ss);
    ASSERT_EQ(msg, std::nullopt);
}

TEST(Parse, BadPriceLeadingSpace) {
    std::string line = "0,1000000,1,10, 10.7";
    std::stringstream ss;
    std::optional<InputMessage> msg = parse(line, ss);
    ASSERT_EQ(msg, std::nullopt);
}

TEST(Parse, MissingPrice) {
    std::string line = "0,1000000,1,10,";
    std::stringstream ss;
    std::optional<InputMessage> msg = parse(line, ss);
    ASSERT_EQ(msg, std::nullopt);
}

TEST(Parse, MissingPriceNoComma) {
    std::string line = "0,1000000,1,10";
    std::stringstream ss;
    std::optional<InputMessage> msg = parse(line, ss);
    ASSERT_EQ(msg, std::nullopt);
}
TEST(Parse, MultipleCommas) {
    std::string line = "0,1000000,1,10,,10.7";
    std::stringstream ss;
    std::optional<InputMessage> msg = parse(line, ss);
    ASSERT_EQ(msg, std::nullopt);
}

} // namespace mukhi::matching_engine