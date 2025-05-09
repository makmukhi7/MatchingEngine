#ifndef MATCHING_ENGINE_ORDER_BOOK_H
#define MATCHING_ENGINE_ORDER_BOOK_H

#include <functional>
#include <list>
#include <map>
#include <unordered_map>

#include "messages.h"

namespace mukhi::matching_engine {

struct Order {
  OrderId id;
  Side side;
  Quantity qty;
  Price price;
};
using OrderList = std::list<Order>;
using SellOrderMap = std::map<Price, OrderList>;
using BuyOrderMap = std::map<Price, OrderList, std::greater<Price>>;
/**
 Note that we don't use a std::variant here since the types
`std::map<PriceOrderList>::iterator` and `std::map<Price, OrderList,
 std::greater<Price>>::iterator` can't be disambigauted. We could technically
use the same type `std::map<Price,OrderList>::iterator` to capture instances of
both but that seems like a risky optimization, so to be on the safer side we use
a struct here.
*/
struct IteratorVariant {
  SellOrderMap::iterator sell_order_map_it;
  BuyOrderMap::iterator buy_order_map_it;
};
using OrderEntry = std::pair<IteratorVariant, OrderList::iterator>;
using PriceIndex = std::unordered_map<Price, IteratorVariant>;

/*
Keeps track of orders that haven't yet been fully filled.

An object of this class keeps references to the streams provided during
construction and expects these streams to stay alive during the lifetime of the
object.

Following are the time complexities of various operations on the order book:

* Inserting a new order: If an order with same price and same type (buy/sell)
already exists in the book then O(1), otherwise O(log(n)).

* Deletion (canceled or fulfilled): O(1) (Note: that complexity of deleting from
a b-tree with an iterator to the node being delted is amortized constant).

* Matching: O(m), where m is the number of resting orders an incoming order
matchies. So determining if there's at least one match is constant time
complexity.

This class is not thread-safe.
*/
class OrderBook {
 public:
  OrderBook(std::ostream& os, std::ostream& es) : os_(os), es_(es) {}

  void ProcessOrder(const AddOrderRequest& req);
  void ProcessOrder(const CancelOrderRequest& req);

 private:
  // Incoming price, resting price -> successful match.
  using MatchingFunction = std::function<bool(Price, Price)>;
  // Match incoming order against resting orders.
  template <typename MapType>
  void MatchOrders(Order& incoming_order, MapType& resting_orders,
                   MatchingFunction match);
  // Add a new order to the book.
  void AddOrder(Order o);
  // Execute trades against the order list of specific price.
  void ExecuteTrades(Order& incoming_order, OrderList& order_list);

  std::ostream& os_;
  std::ostream& es_;

  // Tracks all sell orders and keeps them sorted by price.
  SellOrderMap sell_orders_;
  // Tracks all buy orders and keeps them sorted by price.
  BuyOrderMap buy_orders_;
  // Tracks all orders by id.
  std::unordered_map<OrderId, OrderEntry> order_id_index_;

  /**
   Following map is for optimizing insertion of orders at any price. If there
   exists an order at the same price, insertion can happen in constant time
   instead of the default log(n) of b-tree. Note that we can keep the same map
   for both buy and sell since at a given price only one type of the order can
   be in the book (otherwise they will result in a trade).
   */
  PriceIndex price_index_;

#ifdef UNIT_TEST
  friend class OrderBookTest;
#endif  // UNIT_TEST
};

}  // namespace mukhi::matching_engine

#endif  // MATCHING_ENGINE_ORDER_BOOK_H