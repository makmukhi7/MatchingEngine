#define UNIT_TEST
#include "order_book.h"

#include <gtest/gtest.h>

#include <memory>

// Note that the code in these tests may seem repetitive but keeping with the
// DRY vs DAMP concept we are purposefully repeating code in order to keep
// testing logic simple and very easy to follow.

namespace mukhi::matching_engine {

class OrderBookTest : public ::testing::Test {
 protected:
  void SetUp() { b = std::make_unique<OrderBook>(oss, ess); }

  const SellOrderMap& sell_order_map() const { return b->sell_orders_; }
  const BuyOrderMap& buy_order_map() const { return b->buy_orders_; }
  const std::unordered_map<OrderId, OrderEntry>& order_id_index() const {
    return b->order_id_index_;
  }
  const PriceIndex& price_index() const { return b->price_index_; }

  std::ostringstream oss;
  std::ostringstream ess;
  std::unique_ptr<OrderBook> b;
};

TEST_F(OrderBookTest, SingleInsert) {
  AddOrderRequest req{
      .order_id = 1111, .side = Side::kSell, .qty = 15, .price = 11.0};
  b->ProcessOrder(req);

  // Nothing is printed when no trade occurs.
  EXPECT_EQ(oss.str(), "");

  EXPECT_EQ(sell_order_map().size(), 1);
  EXPECT_EQ(buy_order_map().size(), 0);
  EXPECT_EQ(price_index().size(), 1);
  EXPECT_EQ(order_id_index().size(), 1);
}

TEST_F(OrderBookTest, RepeatedOrderId) {
  AddOrderRequest req{
      .order_id = 1111, .side = Side::kSell, .qty = 15, .price = 11.0};
  b->ProcessOrder(req);

  // Nothing is printed when no trade occurs.
  EXPECT_EQ(oss.str(), "");

  EXPECT_EQ(sell_order_map().size(), 1);
  EXPECT_EQ(buy_order_map().size(), 0);
  EXPECT_EQ(price_index().size(), 1);
  EXPECT_EQ(order_id_index().size(), 1);

  // Repeat the order id.
  b->ProcessOrder(req);

  // Nothing is printed when no trade occurs.
  EXPECT_EQ(oss.str(), "");

  // State remains unchanged.
  EXPECT_EQ(sell_order_map().size(), 1);
  EXPECT_EQ(buy_order_map().size(), 0);
  EXPECT_EQ(price_index().size(), 1);
  EXPECT_EQ(order_id_index().size(), 1);
}

TEST_F(OrderBookTest, SingleCancel) {
  CancelOrderRequest req{.order_id = 999};
  b->ProcessOrder(req);

  // Nothing is printed when no trade occurs.
  EXPECT_EQ(oss.str(), "");

  EXPECT_EQ(sell_order_map().size(), 0);
  EXPECT_EQ(buy_order_map().size(), 0);
  EXPECT_EQ(price_index().size(), 0);
  EXPECT_EQ(order_id_index().size(), 0);
}

TEST_F(OrderBookTest, SingleInsertAndCancelSell) {
  AddOrderRequest add{
      .order_id = 1111, .side = Side::kSell, .qty = 15, .price = 11.0};
  b->ProcessOrder(add);

  // Nothing is printed when no trade occurs.
  EXPECT_EQ(oss.str(), "");

  EXPECT_EQ(sell_order_map().size(), 1);
  EXPECT_EQ(buy_order_map().size(), 0);
  EXPECT_EQ(price_index().size(), 1);
  EXPECT_EQ(order_id_index().size(), 1);

  CancelOrderRequest can{.order_id = 1111};
  b->ProcessOrder(can);

  // Nothing is printed when no trade occurs.
  EXPECT_EQ(oss.str(), "");
  EXPECT_EQ(sell_order_map().size(), 0);
  EXPECT_EQ(buy_order_map().size(), 0);
  EXPECT_EQ(price_index().size(), 0);
  EXPECT_EQ(order_id_index().size(), 0);
}

TEST_F(OrderBookTest, SingleInsertAndCancelBuy) {
  AddOrderRequest add{
      .order_id = 1111, .side = Side::kBuy, .qty = 15, .price = 11.0};
  b->ProcessOrder(add);

  // Nothing is printed when no trade occurs.
  EXPECT_EQ(oss.str(), "");

  EXPECT_EQ(sell_order_map().size(), 0);
  EXPECT_EQ(buy_order_map().size(), 1);
  EXPECT_EQ(price_index().size(), 1);
  EXPECT_EQ(order_id_index().size(), 1);

  CancelOrderRequest can{.order_id = 1111};
  b->ProcessOrder(can);

  // Nothing is printed when no trade occurs.
  EXPECT_EQ(oss.str(), "");
  EXPECT_EQ(sell_order_map().size(), 0);
  EXPECT_EQ(buy_order_map().size(), 0);
  EXPECT_EQ(price_index().size(), 0);
  EXPECT_EQ(order_id_index().size(), 0);
}

TEST_F(OrderBookTest, MultipleSellInserts) {
  AddOrderRequest req1{
      .order_id = 1111, .side = Side::kSell, .qty = 15, .price = 11.0};
  b->ProcessOrder(req1);

  // Nothing is printed when no trade occurs.
  EXPECT_EQ(oss.str(), "");

  EXPECT_EQ(sell_order_map().size(), 1);
  EXPECT_EQ(buy_order_map().size(), 0);
  EXPECT_EQ(price_index().size(), 1);
  EXPECT_EQ(order_id_index().size(), 1);

  AddOrderRequest req2{
      .order_id = 1112, .side = Side::kSell, .qty = 5, .price = 11.0};
  b->ProcessOrder(req2);

  // Nothing is printed when no trade occurs.
  EXPECT_EQ(oss.str(), "");

  EXPECT_EQ(sell_order_map().size(), 1);
  EXPECT_EQ(sell_order_map().begin()->second.size(), 2);
  EXPECT_EQ(buy_order_map().size(), 0);
  EXPECT_EQ(price_index().size(), 1);
  EXPECT_EQ(order_id_index().size(), 2);

  AddOrderRequest req3{
      .order_id = 1115, .side = Side::kSell, .qty = 15, .price = 5.0};
  b->ProcessOrder(req3);

  // Nothing is printed when no trade occurs.
  EXPECT_EQ(oss.str(), "");

  EXPECT_EQ(sell_order_map().size(), 2);
  EXPECT_EQ(buy_order_map().size(), 0);
  EXPECT_EQ(price_index().size(), 2);
  EXPECT_EQ(order_id_index().size(), 3);
}

TEST_F(OrderBookTest, MultipleBuyInserts) {
  AddOrderRequest req1{
      .order_id = 1111, .side = Side::kBuy, .qty = 15, .price = 11.0};
  b->ProcessOrder(req1);

  // Nothing is printed when no trade occurs.
  EXPECT_EQ(oss.str(), "");

  EXPECT_EQ(sell_order_map().size(), 0);
  EXPECT_EQ(buy_order_map().size(), 1);
  EXPECT_EQ(price_index().size(), 1);
  EXPECT_EQ(order_id_index().size(), 1);

  AddOrderRequest req2{
      .order_id = 1112, .side = Side::kBuy, .qty = 5, .price = 11.0};
  b->ProcessOrder(req2);

  // Nothing is printed when no trade occurs.
  EXPECT_EQ(oss.str(), "");

  EXPECT_EQ(sell_order_map().size(), 0);
  EXPECT_EQ(buy_order_map().size(), 1);
  EXPECT_EQ(buy_order_map().begin()->second.size(), 2);
  EXPECT_EQ(price_index().size(), 1);
  EXPECT_EQ(order_id_index().size(), 2);

  AddOrderRequest req3{
      .order_id = 1115, .side = Side::kBuy, .qty = 15, .price = 5.0};
  b->ProcessOrder(req3);

  // Nothing is printed when no trade occurs.
  EXPECT_EQ(oss.str(), "");

  EXPECT_EQ(sell_order_map().size(), 0);
  EXPECT_EQ(buy_order_map().size(), 2);
  EXPECT_EQ(price_index().size(), 2);
  EXPECT_EQ(order_id_index().size(), 3);
}

TEST_F(OrderBookTest, MultipleInsertsAndCancel) {
  AddOrderRequest add1{
      .order_id = 1111, .side = Side::kBuy, .qty = 15, .price = 11.0};
  b->ProcessOrder(add1);

  // Nothing is printed when no trade occurs.
  EXPECT_EQ(oss.str(), "");

  EXPECT_EQ(sell_order_map().size(), 0);
  EXPECT_EQ(buy_order_map().size(), 1);
  EXPECT_EQ(price_index().size(), 1);
  EXPECT_EQ(order_id_index().size(), 1);

  AddOrderRequest add2{
      .order_id = 1112, .side = Side::kBuy, .qty = 5, .price = 11.0};
  b->ProcessOrder(add2);

  // Nothing is printed when no trade occurs.
  EXPECT_EQ(oss.str(), "");

  EXPECT_EQ(sell_order_map().size(), 0);
  EXPECT_EQ(buy_order_map().size(), 1);
  EXPECT_EQ(buy_order_map().begin()->second.size(), 2);
  EXPECT_EQ(price_index().size(), 1);
  EXPECT_EQ(order_id_index().size(), 2);

  CancelOrderRequest can1{.order_id = 1111};
  b->ProcessOrder(can1);

  // Nothing is printed when no trade occurs.
  EXPECT_EQ(oss.str(), "");

  EXPECT_EQ(sell_order_map().size(), 0);
  EXPECT_EQ(buy_order_map().size(), 1);
  EXPECT_EQ(price_index().size(), 1);
  EXPECT_EQ(order_id_index().size(), 1);

  CancelOrderRequest can2{.order_id = 1112};
  b->ProcessOrder(can2);

  // Nothing is printed when no trade occurs.
  EXPECT_EQ(oss.str(), "");

  EXPECT_EQ(sell_order_map().size(), 0);
  EXPECT_EQ(buy_order_map().size(), 0);
  EXPECT_EQ(price_index().size(), 0);
  EXPECT_EQ(order_id_index().size(), 0);
}

TEST_F(OrderBookTest, NoTradeOccurs) {
  AddOrderRequest sell{
      .order_id = 1111, .side = Side::kSell, .qty = 15, .price = 11.0};
  b->ProcessOrder(sell);

  // Nothing is printed when no trade occurs.
  EXPECT_EQ(oss.str(), "");

  EXPECT_EQ(sell_order_map().size(), 1);
  EXPECT_EQ(buy_order_map().size(), 0);
  EXPECT_EQ(price_index().size(), 1);
  EXPECT_EQ(order_id_index().size(), 1);

  AddOrderRequest buy{
      .order_id = 1112, .side = Side::kBuy, .qty = 15, .price = 10.0};
  b->ProcessOrder(buy);

  // Nothing is printed when no trade occurs.
  EXPECT_EQ(oss.str(), "");

  EXPECT_EQ(sell_order_map().size(), 1);
  EXPECT_EQ(buy_order_map().size(), 1);
  EXPECT_EQ(price_index().size(), 2);
  EXPECT_EQ(order_id_index().size(), 2);
}

TEST_F(OrderBookTest, TradeOccursBothFullfilledIncomingBuy) {
  AddOrderRequest sell{
      .order_id = 1111, .side = Side::kSell, .qty = 15, .price = 11.0};
  b->ProcessOrder(sell);

  // Nothing is printed when no trade occurs.
  EXPECT_EQ(oss.str(), "");

  EXPECT_EQ(sell_order_map().size(), 1);
  EXPECT_EQ(buy_order_map().size(), 0);
  EXPECT_EQ(price_index().size(), 1);
  EXPECT_EQ(order_id_index().size(), 1);

  AddOrderRequest buy{
      .order_id = 1112, .side = Side::kBuy, .qty = 15, .price = 12.0};
  b->ProcessOrder(buy);

  // Trade event price is from the resting order.
  TradeEvent te{.qty = 15, .price = 11.0};
  OrderFullyFilled incoming{.order_id = 1112};
  OrderFullyFilled resting{.order_id = 1111};

  std::ostringstream expected;
  expected << te << std::endl << incoming << std::endl << resting << std::endl;

  EXPECT_EQ(oss.str(), expected.str());

  EXPECT_EQ(sell_order_map().size(), 0);
  EXPECT_EQ(buy_order_map().size(), 0);
  EXPECT_EQ(price_index().size(), 0);
  EXPECT_EQ(order_id_index().size(), 0);
}

TEST_F(OrderBookTest, TradeOccursBothFullfilledIncomingSell) {
  AddOrderRequest buy{
      .order_id = 1111, .side = Side::kBuy, .qty = 15, .price = 11.0};
  b->ProcessOrder(buy);

  // Nothing is printed when no trade occurs.
  EXPECT_EQ(oss.str(), "");

  EXPECT_EQ(sell_order_map().size(), 0);
  EXPECT_EQ(buy_order_map().size(), 1);
  EXPECT_EQ(price_index().size(), 1);
  EXPECT_EQ(order_id_index().size(), 1);

  AddOrderRequest sell{
      .order_id = 1112, .side = Side::kSell, .qty = 15, .price = 10.0};
  b->ProcessOrder(sell);

  // Trade event price is from the resting order.
  TradeEvent te{.qty = 15, .price = 11.0};
  OrderFullyFilled incoming{.order_id = 1112};
  OrderFullyFilled resting{.order_id = 1111};

  std::ostringstream expected;
  expected << te << std::endl << incoming << std::endl << resting << std::endl;

  EXPECT_EQ(oss.str(), expected.str());

  EXPECT_EQ(sell_order_map().size(), 0);
  EXPECT_EQ(buy_order_map().size(), 0);
  EXPECT_EQ(price_index().size(), 0);
  EXPECT_EQ(order_id_index().size(), 0);
}

TEST_F(OrderBookTest, TradeOccursRestingSellFullfilled) {
  AddOrderRequest sell{
      .order_id = 1111, .side = Side::kSell, .qty = 15, .price = 11.0};
  b->ProcessOrder(sell);

  // Nothing is printed when no trade occurs.
  EXPECT_EQ(oss.str(), "");

  EXPECT_EQ(sell_order_map().size(), 1);
  EXPECT_EQ(buy_order_map().size(), 0);
  EXPECT_EQ(price_index().size(), 1);
  EXPECT_EQ(order_id_index().size(), 1);

  AddOrderRequest buy{
      .order_id = 1112, .side = Side::kBuy, .qty = 20, .price = 12.0};
  b->ProcessOrder(buy);

  // Trade event price is from the resting order.
  // Trade event quantity is from resting order.
  TradeEvent te{.qty = 15, .price = 11.0};
  OrderPartiallyFilled incoming{.order_id = 1112, .remaining = 5};
  OrderFullyFilled resting{.order_id = 1111};

  std::ostringstream expected;
  expected << te << std::endl << incoming << std::endl << resting << std::endl;

  EXPECT_EQ(oss.str(), expected.str());

  EXPECT_EQ(sell_order_map().size(), 0);
  EXPECT_EQ(buy_order_map().size(), 1);
  EXPECT_EQ(price_index().size(), 1);
  EXPECT_EQ(order_id_index().size(), 1);
}

TEST_F(OrderBookTest, TradeOccursRestingBuyFullfilled) {
  AddOrderRequest buy{
      .order_id = 1111, .side = Side::kBuy, .qty = 15, .price = 11.0};
  b->ProcessOrder(buy);

  // Nothing is printed when no trade occurs.
  EXPECT_EQ(oss.str(), "");

  EXPECT_EQ(sell_order_map().size(), 0);
  EXPECT_EQ(buy_order_map().size(), 1);
  EXPECT_EQ(price_index().size(), 1);
  EXPECT_EQ(order_id_index().size(), 1);

  AddOrderRequest sell{
      .order_id = 1112, .side = Side::kSell, .qty = 20, .price = 9.0};
  b->ProcessOrder(sell);

  // Trade event price is from the resting order.
  // Trade event quantity is from resting order.
  TradeEvent te{.qty = 15, .price = 11.0};
  OrderPartiallyFilled incoming{.order_id = 1112, .remaining = 5};
  OrderFullyFilled resting{.order_id = 1111};

  std::ostringstream expected;
  expected << te << std::endl << incoming << std::endl << resting << std::endl;

  EXPECT_EQ(oss.str(), expected.str());

  EXPECT_EQ(sell_order_map().size(), 1);
  EXPECT_EQ(buy_order_map().size(), 0);
  EXPECT_EQ(price_index().size(), 1);
  EXPECT_EQ(order_id_index().size(), 1);
}

TEST_F(OrderBookTest, TradeOccursIncomingBuyFullfilled) {
  AddOrderRequest sell{
      .order_id = 1111, .side = Side::kSell, .qty = 15, .price = 11.0};
  b->ProcessOrder(sell);

  // Nothing is printed when no trade occurs.
  EXPECT_EQ(oss.str(), "");

  EXPECT_EQ(sell_order_map().size(), 1);
  EXPECT_EQ(buy_order_map().size(), 0);
  EXPECT_EQ(price_index().size(), 1);
  EXPECT_EQ(order_id_index().size(), 1);

  AddOrderRequest buy{
      .order_id = 1112, .side = Side::kBuy, .qty = 5, .price = 12.0};
  b->ProcessOrder(buy);

  // Trade event price is from the resting order.
  // Trade event quantity is from incoming order.
  TradeEvent te{.qty = 5, .price = 11.0};
  OrderFullyFilled incoming{.order_id = 1112};
  OrderPartiallyFilled resting{.order_id = 1111, .remaining = 10};

  std::ostringstream expected;
  expected << te << std::endl << incoming << std::endl << resting << std::endl;

  EXPECT_EQ(oss.str(), expected.str());

  EXPECT_EQ(sell_order_map().size(), 1);
  EXPECT_EQ(buy_order_map().size(), 0);
  EXPECT_EQ(price_index().size(), 1);
  EXPECT_EQ(order_id_index().size(), 1);
}

TEST_F(OrderBookTest, TradeOccursIncomingSellFullfilled) {
  AddOrderRequest buy{
      .order_id = 1111, .side = Side::kBuy, .qty = 15, .price = 11.0};
  b->ProcessOrder(buy);

  // Nothing is printed when no trade occurs.
  EXPECT_EQ(oss.str(), "");

  EXPECT_EQ(sell_order_map().size(), 0);
  EXPECT_EQ(buy_order_map().size(), 1);
  EXPECT_EQ(price_index().size(), 1);
  EXPECT_EQ(order_id_index().size(), 1);

  AddOrderRequest sell{
      .order_id = 1112, .side = Side::kSell, .qty = 5, .price = 11.0};
  b->ProcessOrder(sell);

  // Trade event price is from the resting order.
  // Trade event quantity is from incoming order.
  TradeEvent te{.qty = 5, .price = 11.0};
  OrderFullyFilled incoming{.order_id = 1112};
  OrderPartiallyFilled resting{.order_id = 1111, .remaining = 10};

  std::ostringstream expected;
  expected << te << std::endl << incoming << std::endl << resting << std::endl;

  EXPECT_EQ(oss.str(), expected.str());

  EXPECT_EQ(sell_order_map().size(), 0);
  EXPECT_EQ(buy_order_map().size(), 1);
  EXPECT_EQ(price_index().size(), 1);
  EXPECT_EQ(order_id_index().size(), 1);
}

TEST_F(OrderBookTest, IncomingBuyOrderMatchMultipleRestingOrdersFullfills) {
  AddOrderRequest sell1{
      .order_id = 1111, .side = Side::kSell, .qty = 15, .price = 11.0};
  b->ProcessOrder(sell1);

  // Nothing is printed when no trade occurs.
  EXPECT_EQ(oss.str(), "");

  EXPECT_EQ(sell_order_map().size(), 1);
  EXPECT_EQ(buy_order_map().size(), 0);
  EXPECT_EQ(price_index().size(), 1);
  EXPECT_EQ(order_id_index().size(), 1);

  AddOrderRequest sell2{
      .order_id = 1113, .side = Side::kSell, .qty = 5, .price = 10.0};
  b->ProcessOrder(sell2);

  // Nothing is printed when no trade occurs.
  EXPECT_EQ(oss.str(), "");

  EXPECT_EQ(sell_order_map().size(), 2);
  EXPECT_EQ(buy_order_map().size(), 0);
  EXPECT_EQ(price_index().size(), 2);
  EXPECT_EQ(order_id_index().size(), 2);

  AddOrderRequest buy{
      .order_id = 1112, .side = Side::kBuy, .qty = 20, .price = 12.0};
  b->ProcessOrder(buy);

  std::ostringstream expected;
  // 2 Trade events
  TradeEvent te1{.qty = 5, .price = 10.0};
  OrderPartiallyFilled incoming_partial{.order_id = 1112, .remaining = 15};
  OrderFullyFilled resting_full1{.order_id = 1113};

  expected << te1 << std::endl
           << incoming_partial << std::endl
           << resting_full1 << std::endl;

  TradeEvent te2{.qty = 15, .price = 11.0};
  OrderFullyFilled incoming_full{.order_id = 1112};
  OrderFullyFilled resting_full2{.order_id = 1111};

  expected << te2 << std::endl
           << incoming_full << std::endl
           << resting_full2 << std::endl;

  EXPECT_EQ(oss.str(), expected.str());

  EXPECT_EQ(sell_order_map().size(), 0);
  EXPECT_EQ(buy_order_map().size(), 0);
  EXPECT_EQ(price_index().size(), 0);
  EXPECT_EQ(order_id_index().size(), 0);
}

TEST_F(OrderBookTest, IncomingSellOrderMatchMultipleRestingOrdersFullfills) {
  AddOrderRequest buy1{
      .order_id = 1111, .side = Side::kBuy, .qty = 15, .price = 11.0};
  b->ProcessOrder(buy1);

  // Nothing is printed when no trade occurs.
  EXPECT_EQ(oss.str(), "");

  EXPECT_EQ(sell_order_map().size(), 0);
  EXPECT_EQ(buy_order_map().size(), 1);
  EXPECT_EQ(price_index().size(), 1);
  EXPECT_EQ(order_id_index().size(), 1);

  AddOrderRequest buy2{
      .order_id = 1113, .side = Side::kBuy, .qty = 5, .price = 12.0};
  b->ProcessOrder(buy2);

  // Nothing is printed when no trade occurs.
  EXPECT_EQ(oss.str(), "");

  EXPECT_EQ(sell_order_map().size(), 0);
  EXPECT_EQ(buy_order_map().size(), 2);
  EXPECT_EQ(price_index().size(), 2);
  EXPECT_EQ(order_id_index().size(), 2);

  AddOrderRequest sell{
      .order_id = 1112, .side = Side::kSell, .qty = 20, .price = 10.0};
  b->ProcessOrder(sell);

  std::ostringstream expected;
  // 2 Trade events
  TradeEvent te1{.qty = 5, .price = 12.0};
  OrderPartiallyFilled incoming_partial{.order_id = 1112, .remaining = 15};
  OrderFullyFilled resting_full1{.order_id = 1113};

  expected << te1 << std::endl
           << incoming_partial << std::endl
           << resting_full1 << std::endl;

  TradeEvent te2{.qty = 15, .price = 11.0};
  OrderFullyFilled incoming_full{.order_id = 1112};
  OrderFullyFilled resting_full2{.order_id = 1111};

  expected << te2 << std::endl
           << incoming_full << std::endl
           << resting_full2 << std::endl;

  EXPECT_EQ(oss.str(), expected.str());

  EXPECT_EQ(sell_order_map().size(), 0);
  EXPECT_EQ(buy_order_map().size(), 0);
  EXPECT_EQ(price_index().size(), 0);
  EXPECT_EQ(order_id_index().size(), 0);
}

TEST_F(OrderBookTest, MultipleRestingSellOrdersSamePriceOneExecutes) {
  AddOrderRequest sell1{
      .order_id = 1111, .side = Side::kSell, .qty = 15, .price = 11.0};
  b->ProcessOrder(sell1);

  // Nothing is printed when no trade occurs.
  EXPECT_EQ(oss.str(), "");

  EXPECT_EQ(sell_order_map().size(), 1);
  EXPECT_EQ(buy_order_map().size(), 0);
  EXPECT_EQ(price_index().size(), 1);
  EXPECT_EQ(order_id_index().size(), 1);

  AddOrderRequest sell2{
      .order_id = 1113, .side = Side::kSell, .qty = 5, .price = 11.0};
  b->ProcessOrder(sell2);

  // Nothing is printed when no trade occurs.
  EXPECT_EQ(oss.str(), "");

  EXPECT_EQ(sell_order_map().size(), 1);
  EXPECT_EQ(sell_order_map().begin()->second.size(), 2);
  EXPECT_EQ(buy_order_map().size(), 0);
  EXPECT_EQ(price_index().size(), 1);
  EXPECT_EQ(order_id_index().size(), 2);

  AddOrderRequest buy{
      .order_id = 1112, .side = Side::kBuy, .qty = 15, .price = 12.0};
  b->ProcessOrder(buy);

  std::ostringstream expected;
  // 1 Trade event
  TradeEvent te{.qty = 15, .price = 11.0};
  OrderFullyFilled incoming{.order_id = 1112};
  OrderFullyFilled resting{.order_id = 1111};

  expected << te << std::endl << incoming << std::endl << resting << std::endl;

  EXPECT_EQ(oss.str(), expected.str());

  EXPECT_EQ(sell_order_map().size(), 1);
  EXPECT_EQ(buy_order_map().size(), 0);
  EXPECT_EQ(price_index().size(), 1);
  EXPECT_EQ(order_id_index().size(), 1);
}

TEST_F(OrderBookTest, MultipleRestingBuyOrdersSamePriceOneExecutes) {
  AddOrderRequest buy1{
      .order_id = 1111, .side = Side::kBuy, .qty = 15, .price = 11.0};
  b->ProcessOrder(buy1);

  // Nothing is printed when no trade occurs.
  EXPECT_EQ(oss.str(), "");

  EXPECT_EQ(sell_order_map().size(), 0);
  EXPECT_EQ(buy_order_map().size(), 1);
  EXPECT_EQ(price_index().size(), 1);
  EXPECT_EQ(order_id_index().size(), 1);

  AddOrderRequest buy2{
      .order_id = 1113, .side = Side::kBuy, .qty = 5, .price = 11.0};
  b->ProcessOrder(buy2);

  // Nothing is printed when no trade occurs.
  EXPECT_EQ(oss.str(), "");

  EXPECT_EQ(sell_order_map().size(), 0);
  EXPECT_EQ(buy_order_map().size(), 1);
  EXPECT_EQ(buy_order_map().begin()->second.size(), 2);
  EXPECT_EQ(price_index().size(), 1);
  EXPECT_EQ(order_id_index().size(), 2);

  AddOrderRequest sell{
      .order_id = 1112, .side = Side::kSell, .qty = 15, .price = 9.0};
  b->ProcessOrder(sell);

  std::ostringstream expected;
  // 1 Trade event
  TradeEvent te{.qty = 15, .price = 11.0};
  OrderFullyFilled incoming{.order_id = 1112};
  OrderFullyFilled resting{.order_id = 1111};

  expected << te << std::endl << incoming << std::endl << resting << std::endl;

  EXPECT_EQ(oss.str(), expected.str());

  EXPECT_EQ(sell_order_map().size(), 0);
  EXPECT_EQ(buy_order_map().size(), 1);
  EXPECT_EQ(price_index().size(), 1);
  EXPECT_EQ(order_id_index().size(), 1);
}

}  // namespace mukhi::matching_engine