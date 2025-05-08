#include "order_book.h"

#include <functional>
#include <iostream>

namespace mukhi::matching_engine {
namespace {
template <typename MapType, typename MapIteratorType>
void RemoveFromOrderMap(MapType& m, MapIteratorType map_itr,
                        OrderList::iterator order_list_itr,
                        PriceIndex& price_index) {
  if (map_itr->second.size() == 1) {
    // If there's only one order for that price, we can remove the map entry
    // itself. And also remove from price index.
    price_index.erase(map_itr->first);
    m.erase(map_itr);
  } else {
    // Remove the order from the `OrderList`.
    map_itr->second.erase(order_list_itr);
  }
}

template <typename MapType, typename MapTypeIterator>
std::pair<MapTypeIterator, OrderList::iterator> AddToOrderMap(MapType& m,
                                                              Order o) {
  auto [order_map_itr, success] =
      m.emplace(std::make_pair(o.price, OrderList()));
  OrderList& order_list = order_map_itr->second;
  OrderList::iterator order_list_itr =
      order_list.insert(order_list.end(), std::move(o));
  return {order_map_itr, order_list_itr};
}

// Following helper functions help keep matching logic agnostic of if the
// incoming order is buy or sell.
bool IncomingSellMatcher(Price incoming, Price resting) {
  return incoming <= resting;
}
bool IncomingBuyMatcher(Price incoming, Price resting) {
  return resting <= incoming;
}

}  // namespace
void OrderBook::ExecuteTrades(Order& incoming_order, OrderList& order_list) {
  while (incoming_order.qty > 0 && !order_list.empty()) {
    Order& resting_order = order_list.front();
    TradeEvent te;
    te.qty = std::min(incoming_order.qty, resting_order.qty);
    // Price of the resting order is trade event's price
    te.price = resting_order.price;
    // Generate messages
    os_ << te << std::endl;
    if (te.qty == incoming_order.qty) {
      OrderFullyFilled o{.order_id = incoming_order.id};
      os_ << o << std::endl;
      incoming_order.qty = 0;
    } else {
      incoming_order.qty -= te.qty;
      OrderPartiallyFilled o{.order_id = incoming_order.id,
                             .remaining = incoming_order.qty};
      os_ << o << std::endl;
    }
    if (te.qty == resting_order.qty) {
      OrderFullyFilled o{.order_id = resting_order.id};
      os_ << o << std::endl;
      // Remove resting order from the book.
      order_id_index_.erase(resting_order.id);
      order_list.pop_front();
    } else {
      resting_order.qty -= te.qty;
      OrderPartiallyFilled o{.order_id = resting_order.id,
                             .remaining = resting_order.qty};
      os_ << o << std::endl;
    }
  }
}

template <typename MapType>
void OrderBook::MatchOrders(Order& incoming_order, MapType& resting_orders,
                            MatchingFunction match) {
  for (auto itr = resting_orders.begin();
       itr != resting_orders.end() && incoming_order.qty > 0;) {
    Price resting_price = itr->first;
    if (!match(incoming_order.price, resting_price)) break;

    OrderList& order_list = itr->second;
    ExecuteTrades(incoming_order, order_list);
    if (order_list.empty()) {
      // Remove this resting price from order book.
      price_index_.erase(resting_price);
      itr = resting_orders.erase(itr);
    } else {
      ++itr;
    }
  }
}

void OrderBook::AddOrder(Order o) {
  auto price_index_itr = price_index_.find(o.price);
  if (price_index_itr != price_index_.end()) {
    // An order list for this price already exists.
    IteratorVariant order_map_itr = price_index_itr->second;
    OrderList::iterator order_list_itr;
    OrderList* list;
    if (o.side == Side::kSell) {
      list = &order_map_itr.sell_order_map_it->second;
    } else {
      list = &order_map_itr.buy_order_map_it->second;
    }
    order_list_itr = list->insert(list->end(), std::move(o));
    order_id_index_.emplace(
        std::make_pair(o.id, OrderEntry{order_map_itr, order_list_itr}));
  } else {
    Price price = o.price;
    OrderId id = o.id;
    OrderEntry entry;
    if (o.side == Side::kSell) {
      auto [map_itr, list_itr] =
          AddToOrderMap<SellOrderMap, SellOrderMap::iterator>(sell_orders_,
                                                              std::move(o));
      entry.first.sell_order_map_it = map_itr;
      entry.second = list_itr;
    } else {
      auto [map_itr, list_itr] =
          AddToOrderMap<BuyOrderMap, BuyOrderMap::iterator>(buy_orders_,
                                                            std::move(o));
      entry.first.buy_order_map_it = map_itr;
      entry.second = list_itr;
    }
    price_index_.emplace(std::make_pair(price, entry.first));
    order_id_index_.emplace(std::make_pair(id, std::move(entry)));
  }
}

void OrderBook::ProcessOrder(const AddOrderRequest& req) {
  // Check that order id isn't being repeated
  auto order_id_index_itr = order_id_index_.find(req.order_id);
  if (order_id_index_itr != order_id_index_.end()) {
    es_ << "Unable to process: Order id is being repeated: " << req.order_id
        << std::endl;
    return;
  }

  Order incoming_order{
      .id = req.order_id, .side = req.side, .qty = req.qty, .price = req.price};
  if (incoming_order.side == Side::kSell) {
    MatchOrders(incoming_order, buy_orders_, IncomingSellMatcher);
  } else {
    MatchOrders(incoming_order, sell_orders_, IncomingBuyMatcher);
  }
  if (incoming_order.qty > 0) {
    AddOrder(std::move(incoming_order));
  }
}

void OrderBook::ProcessOrder(const CancelOrderRequest& req) {
  auto order_id_index_itr = order_id_index_.find(req.order_id);
  if (order_id_index_itr == order_id_index_.end()) {
    es_ << "No such order with id: " << req.order_id << std::endl;
    return;
  }
  IteratorVariant order_map_itr = order_id_index_itr->second.first;
  OrderList::iterator order_list_itr = order_id_index_itr->second.second;

  // Remove from order id index
  order_id_index_.erase(order_id_index_itr);

  // Remove from order list or the order map
  if (order_list_itr->side == Side::kBuy) {
    RemoveFromOrderMap(buy_orders_, order_map_itr.buy_order_map_it,
                       order_list_itr, price_index_);
  } else {
    RemoveFromOrderMap(sell_orders_, order_map_itr.sell_order_map_it,
                       order_list_itr, price_index_);
  }
}

}  // namespace mukhi::matching_engine