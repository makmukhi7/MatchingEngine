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

} // namespace mukhi::matching_engine