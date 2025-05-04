#include "messages.h"

#include <gtest/gtest.h>

namespace mukhi::matching_engine
{
    TEST(TradeEvent, to_string)
    {
        TradeEvent te{
            .qty = 10,
            .price = 15.0,
        };
        EXPECT_EQ(te.to_string(), "2,10,15");
    }

    TEST(OrderFullyFilled, to_string)
    {
        OrderFullyFilled of{.order_id = 100000};

        EXPECT_EQ(of.to_string(), "3,100000");
    }

    TEST(OrderPartiallyFilled, to_string)
    {
        OrderPartiallyFilled opf{.order_id = 1000001, .remaining = 75};

        EXPECT_EQ(opf.to_string(), "4,1000001,75");
    }

} // namespace mukhi::matching_engine