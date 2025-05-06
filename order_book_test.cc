#include "order_book.h"

#include <gtest/gtest.h>

// Note that the code in these tests may seem repetitive but keeping with the DRY vs DAMP concept we are purposefully repeating code in order to keep testing logic simple and very easy to follow.


namespace mukhi::matching_engine {
TEST(OrderBook, SingleInsert) {
    std::ostringstream oss;
    OrderBook b(oss);
    AddOrderRequest req{ .order_id = 1111, .side = Side::kSell, .qty = 15, .price = 11.0 };
    b.ProcessOrder(req);

    // Nothing is printed when no trade occurs.
    EXPECT_EQ(oss.str(), "");

    EXPECT_EQ(b.TEST_sell_order_map().size(), 1);
    EXPECT_EQ(b.TEST_buy_order_map().size(), 0);
    EXPECT_EQ(b.TEST_price_index().size(), 1);
    EXPECT_EQ(b.TEST_order_id_index().size(), 1);
}

TEST(OrderBook, RepeatedOrderId) {
    std::ostringstream oss;
    OrderBook b(oss);
    AddOrderRequest req{ .order_id = 1111, .side = Side::kSell, .qty = 15, .price = 11.0 };
    b.ProcessOrder(req);

    // Nothing is printed when no trade occurs.
    EXPECT_EQ(oss.str(), "");

    EXPECT_EQ(b.TEST_sell_order_map().size(), 1);
    EXPECT_EQ(b.TEST_buy_order_map().size(), 0);
    EXPECT_EQ(b.TEST_price_index().size(), 1);
    EXPECT_EQ(b.TEST_order_id_index().size(), 1);

    // Repeat the order id.
    b.ProcessOrder(req);

    // Nothing is printed when no trade occurs.
    EXPECT_EQ(oss.str(), "");

    // State remains unchanged.
    EXPECT_EQ(b.TEST_sell_order_map().size(), 1);
    EXPECT_EQ(b.TEST_buy_order_map().size(), 0);
    EXPECT_EQ(b.TEST_price_index().size(), 1);
    EXPECT_EQ(b.TEST_order_id_index().size(), 1);

}

TEST(OrderBook, SingleCancel) {
    std::ostringstream oss;
    OrderBook b(oss);
    CancelOrderRequest req{ .order_id = 999 };
    b.ProcessOrder(req);

    // Nothing is printed when no trade occurs.
    EXPECT_EQ(oss.str(), "");

    EXPECT_EQ(b.TEST_sell_order_map().size(), 0);
    EXPECT_EQ(b.TEST_buy_order_map().size(), 0);
    EXPECT_EQ(b.TEST_price_index().size(), 0);
    EXPECT_EQ(b.TEST_order_id_index().size(), 0);
}

TEST(OrderBook, SingleInsertAndCancel) {
    std::ostringstream oss;
    OrderBook b(oss);
    AddOrderRequest add{ .order_id = 1111, .side = Side::kSell, .qty = 15, .price = 11.0 };
    b.ProcessOrder(add);

    // Nothing is printed when no trade occurs.
    EXPECT_EQ(oss.str(), "");

    EXPECT_EQ(b.TEST_sell_order_map().size(), 1);
    EXPECT_EQ(b.TEST_buy_order_map().size(), 0);
    EXPECT_EQ(b.TEST_price_index().size(), 1);
    EXPECT_EQ(b.TEST_order_id_index().size(), 1);

    CancelOrderRequest can{ .order_id = 1111 };
    b.ProcessOrder(can);

    // Nothing is printed when no trade occurs.
    EXPECT_EQ(oss.str(), "");
    EXPECT_EQ(b.TEST_sell_order_map().size(), 0);
    EXPECT_EQ(b.TEST_buy_order_map().size(), 0);
    EXPECT_EQ(b.TEST_price_index().size(), 0);
    EXPECT_EQ(b.TEST_order_id_index().size(), 0);
}

TEST(OrderBook, MultipleSellInserts) {
    std::ostringstream oss;
    OrderBook b(oss);
    AddOrderRequest req1{ .order_id = 1111, .side = Side::kSell, .qty = 15, .price = 11.0 };
    b.ProcessOrder(req1);

    // Nothing is printed when no trade occurs.
    EXPECT_EQ(oss.str(), "");

    EXPECT_EQ(b.TEST_sell_order_map().size(), 1);
    EXPECT_EQ(b.TEST_buy_order_map().size(), 0);
    EXPECT_EQ(b.TEST_price_index().size(), 1);
    EXPECT_EQ(b.TEST_order_id_index().size(), 1);

    AddOrderRequest req2{ .order_id = 1112, .side = Side::kSell, .qty = 5, .price = 11.0 };
    b.ProcessOrder(req2);

    // Nothing is printed when no trade occurs.
    EXPECT_EQ(oss.str(), "");

    EXPECT_EQ(b.TEST_sell_order_map().size(), 1);
    EXPECT_EQ(b.TEST_sell_order_map().begin()->second.size(), 2);
    EXPECT_EQ(b.TEST_buy_order_map().size(), 0);
    EXPECT_EQ(b.TEST_price_index().size(), 1);
    EXPECT_EQ(b.TEST_order_id_index().size(), 2);

    AddOrderRequest req3{ .order_id = 1115, .side = Side::kSell, .qty = 15, .price = 5.0 };
    b.ProcessOrder(req3);

    // Nothing is printed when no trade occurs.
    EXPECT_EQ(oss.str(), "");

    EXPECT_EQ(b.TEST_sell_order_map().size(), 2);
    EXPECT_EQ(b.TEST_buy_order_map().size(), 0);
    EXPECT_EQ(b.TEST_price_index().size(), 2);
    EXPECT_EQ(b.TEST_order_id_index().size(), 3);
}

TEST(OrderBook, MultipleBuyInserts) {
    std::ostringstream oss;
    OrderBook b(oss);
    AddOrderRequest req1{ .order_id = 1111, .side = Side::kBuy, .qty = 15, .price = 11.0 };
    b.ProcessOrder(req1);

    // Nothing is printed when no trade occurs.
    EXPECT_EQ(oss.str(), "");

    EXPECT_EQ(b.TEST_sell_order_map().size(), 0);
    EXPECT_EQ(b.TEST_buy_order_map().size(), 1);
    EXPECT_EQ(b.TEST_price_index().size(), 1);
    EXPECT_EQ(b.TEST_order_id_index().size(), 1);

    AddOrderRequest req2{ .order_id = 1112, .side = Side::kBuy, .qty = 5, .price = 11.0 };
    b.ProcessOrder(req2);

    // Nothing is printed when no trade occurs.
    EXPECT_EQ(oss.str(), "");

    EXPECT_EQ(b.TEST_sell_order_map().size(), 0);
    EXPECT_EQ(b.TEST_buy_order_map().size(), 1);
    EXPECT_EQ(b.TEST_buy_order_map().begin()->second.size(), 2);
    EXPECT_EQ(b.TEST_price_index().size(), 1);
    EXPECT_EQ(b.TEST_order_id_index().size(), 2);

    AddOrderRequest req3{ .order_id = 1115, .side = Side::kBuy, .qty = 15, .price = 5.0 };
    b.ProcessOrder(req3);

    // Nothing is printed when no trade occurs.
    EXPECT_EQ(oss.str(), "");

    EXPECT_EQ(b.TEST_sell_order_map().size(), 0);
    EXPECT_EQ(b.TEST_buy_order_map().size(), 2);
    EXPECT_EQ(b.TEST_price_index().size(), 2);
    EXPECT_EQ(b.TEST_order_id_index().size(), 3);
}

TEST(OrderBook, MultipleInsertsAndCancel) {
    std::ostringstream oss;
    OrderBook b(oss);
    AddOrderRequest req1{ .order_id = 1111, .side = Side::kBuy, .qty = 15, .price = 11.0 };
    b.ProcessOrder(req1);

    // Nothing is printed when no trade occurs.
    EXPECT_EQ(oss.str(), "");

    EXPECT_EQ(b.TEST_sell_order_map().size(), 0);
    EXPECT_EQ(b.TEST_buy_order_map().size(), 1);
    EXPECT_EQ(b.TEST_price_index().size(), 1);
    EXPECT_EQ(b.TEST_order_id_index().size(), 1);

    AddOrderRequest req2{ .order_id = 1112, .side = Side::kBuy, .qty = 5, .price = 11.0 };
    b.ProcessOrder(req2);

    // Nothing is printed when no trade occurs.
    EXPECT_EQ(oss.str(), "");

    EXPECT_EQ(b.TEST_sell_order_map().size(), 0);
    EXPECT_EQ(b.TEST_buy_order_map().size(), 1);
    EXPECT_EQ(b.TEST_buy_order_map().begin()->second.size(), 2);
    EXPECT_EQ(b.TEST_price_index().size(), 1);
    EXPECT_EQ(b.TEST_order_id_index().size(), 2);

    CancelOrderRequest can{ .order_id = 1111 };
    b.ProcessOrder(can);

    // Nothing is printed when no trade occurs.
    EXPECT_EQ(oss.str(), "");

    EXPECT_EQ(b.TEST_sell_order_map().size(), 0);
    EXPECT_EQ(b.TEST_buy_order_map().size(), 1);
    EXPECT_EQ(b.TEST_price_index().size(), 1);
    EXPECT_EQ(b.TEST_order_id_index().size(), 1);
}

TEST(OrderBook, NoTradeOccurs) {
    std::ostringstream oss;
    OrderBook b(oss);
    AddOrderRequest sell{ .order_id = 1111, .side = Side::kSell, .qty = 15, .price = 11.0 };
    b.ProcessOrder(sell);

    // Nothing is printed when no trade occurs.
    EXPECT_EQ(oss.str(), "");

    EXPECT_EQ(b.TEST_sell_order_map().size(), 1);
    EXPECT_EQ(b.TEST_buy_order_map().size(), 0);
    EXPECT_EQ(b.TEST_price_index().size(), 1);
    EXPECT_EQ(b.TEST_order_id_index().size(), 1);

    AddOrderRequest buy{ .order_id = 1112, .side = Side::kBuy, .qty = 15, .price = 10.0 };
    b.ProcessOrder(buy);

    // Nothing is printed when no trade occurs.
    EXPECT_EQ(oss.str(), "");

    EXPECT_EQ(b.TEST_sell_order_map().size(), 1);
    EXPECT_EQ(b.TEST_buy_order_map().size(), 1);
    EXPECT_EQ(b.TEST_price_index().size(), 2);
    EXPECT_EQ(b.TEST_order_id_index().size(), 2);
}

TEST(OrderBook, TradeOccursBothFullfilled) {
    std::ostringstream oss;
    OrderBook b(oss);
    AddOrderRequest sell{ .order_id = 1111, .side = Side::kSell, .qty = 15, .price = 11.0 };
    b.ProcessOrder(sell);

    // Nothing is printed when no trade occurs.
    EXPECT_EQ(oss.str(), "");

    EXPECT_EQ(b.TEST_sell_order_map().size(), 1);
    EXPECT_EQ(b.TEST_buy_order_map().size(), 0);
    EXPECT_EQ(b.TEST_price_index().size(), 1);
    EXPECT_EQ(b.TEST_order_id_index().size(), 1);

    AddOrderRequest buy{ .order_id = 1112, .side = Side::kBuy, .qty = 15, .price = 12.0 };
    b.ProcessOrder(buy);

    // Trade event price is from the resting order.
    TradeEvent te{ .qty = 15, .price = 11.0 };
    OrderFullyFilled incoming{ .order_id = 1112 };
    OrderFullyFilled resting{ .order_id = 1111 };

    std::ostringstream expected;
    expected << te << std::endl << incoming << std::endl << resting << std::endl;

    EXPECT_EQ(oss.str(), expected.str());

    EXPECT_EQ(b.TEST_sell_order_map().size(), 0);
    EXPECT_EQ(b.TEST_buy_order_map().size(), 0);
    EXPECT_EQ(b.TEST_price_index().size(), 0);
    EXPECT_EQ(b.TEST_order_id_index().size(), 0);
}

TEST(OrderBook, TradeOccursRestingFullfilled) {
    std::ostringstream oss;
    OrderBook b(oss);
    AddOrderRequest sell{ .order_id = 1111, .side = Side::kSell, .qty = 15, .price = 11.0 };
    b.ProcessOrder(sell);

    // Nothing is printed when no trade occurs.
    EXPECT_EQ(oss.str(), "");

    EXPECT_EQ(b.TEST_sell_order_map().size(), 1);
    EXPECT_EQ(b.TEST_buy_order_map().size(), 0);
    EXPECT_EQ(b.TEST_price_index().size(), 1);
    EXPECT_EQ(b.TEST_order_id_index().size(), 1);

    AddOrderRequest buy{ .order_id = 1112, .side = Side::kBuy, .qty = 20, .price = 12.0 };
    b.ProcessOrder(buy);

    // Trade event price is from the resting order.
    // Trade event quantity is from resting order.
    TradeEvent te{ .qty = 15, .price = 11.0 };
    OrderPartiallyFilled incoming{ .order_id = 1112, .remaining = 5 };
    OrderFullyFilled resting{ .order_id = 1111 };

    std::ostringstream expected;
    expected << te << std::endl << incoming << std::endl << resting << std::endl;

    EXPECT_EQ(oss.str(), expected.str());

    EXPECT_EQ(b.TEST_sell_order_map().size(), 0);
    EXPECT_EQ(b.TEST_buy_order_map().size(), 1);
    EXPECT_EQ(b.TEST_price_index().size(), 1);
    EXPECT_EQ(b.TEST_order_id_index().size(), 1);
}

TEST(OrderBook, TradeOccursIncomingFullfilled) {
    std::ostringstream oss;
    OrderBook b(oss);
    AddOrderRequest sell{ .order_id = 1111, .side = Side::kSell, .qty = 15, .price = 11.0 };
    b.ProcessOrder(sell);

    // Nothing is printed when no trade occurs.
    EXPECT_EQ(oss.str(), "");

    EXPECT_EQ(b.TEST_sell_order_map().size(), 1);
    EXPECT_EQ(b.TEST_buy_order_map().size(), 0);
    EXPECT_EQ(b.TEST_price_index().size(), 1);
    EXPECT_EQ(b.TEST_order_id_index().size(), 1);

    AddOrderRequest buy{ .order_id = 1112, .side = Side::kBuy, .qty = 5, .price = 12.0 };
    b.ProcessOrder(buy);

    // Trade event price is from the resting order.
    // Trade event quantity is from incoming order.
    TradeEvent te{ .qty = 5, .price = 11.0 };
    OrderFullyFilled incoming{ .order_id = 1112 };
    OrderPartiallyFilled resting{ .order_id = 1111, .remaining = 10 };

    std::ostringstream expected;
    expected << te << std::endl << incoming << std::endl << resting << std::endl;

    EXPECT_EQ(oss.str(), expected.str());

    EXPECT_EQ(b.TEST_sell_order_map().size(), 1);
    EXPECT_EQ(b.TEST_buy_order_map().size(), 0);
    EXPECT_EQ(b.TEST_price_index().size(), 1);
    EXPECT_EQ(b.TEST_order_id_index().size(), 1);
}

TEST(OrderBook, IncomingOrderMatchMultipleRestingOrdersFullfills) {
    std::ostringstream oss;
    OrderBook b(oss);
    AddOrderRequest sell1{ .order_id = 1111, .side = Side::kSell, .qty = 15, .price = 11.0 };
    b.ProcessOrder(sell1);

    // Nothing is printed when no trade occurs.
    EXPECT_EQ(oss.str(), "");

    EXPECT_EQ(b.TEST_sell_order_map().size(), 1);
    EXPECT_EQ(b.TEST_buy_order_map().size(), 0);
    EXPECT_EQ(b.TEST_price_index().size(), 1);
    EXPECT_EQ(b.TEST_order_id_index().size(), 1);

    AddOrderRequest sell2{ .order_id = 1113, .side = Side::kSell, .qty = 5, .price = 10.0 };
    b.ProcessOrder(sell2);

    // Nothing is printed when no trade occurs.
    EXPECT_EQ(oss.str(), "");

    EXPECT_EQ(b.TEST_sell_order_map().size(), 2);
    EXPECT_EQ(b.TEST_buy_order_map().size(), 0);
    EXPECT_EQ(b.TEST_price_index().size(), 2);
    EXPECT_EQ(b.TEST_order_id_index().size(), 2);

    AddOrderRequest buy{ .order_id = 1112, .side = Side::kBuy, .qty = 20, .price = 12.0 };
    b.ProcessOrder(buy);

    std::ostringstream expected;
    // 2 Trade events
    TradeEvent te1{ .qty = 5, .price = 10.0 };
    OrderPartiallyFilled incoming_partial{ .order_id = 1112, .remaining = 15 };
    OrderFullyFilled resting_full1{ .order_id = 1113 };

    expected << te1 << std::endl << incoming_partial << std::endl << resting_full1 << std::endl;

    TradeEvent te2{ .qty = 15, .price = 11.0 };
    OrderFullyFilled incoming_full{ .order_id = 1112 };
    OrderFullyFilled resting_full2{ .order_id = 1111 };

    expected << te2 << std::endl << incoming_full << std::endl << resting_full2 << std::endl;

    EXPECT_EQ(oss.str(), expected.str());

    EXPECT_EQ(b.TEST_sell_order_map().size(), 0);
    EXPECT_EQ(b.TEST_buy_order_map().size(), 0);
    EXPECT_EQ(b.TEST_price_index().size(), 0);
    EXPECT_EQ(b.TEST_order_id_index().size(), 0);
}

}