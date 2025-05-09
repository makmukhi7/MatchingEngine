#ifndef MATCHING_ENGINE_MATCHING_ENGINE_H
#define MATCHING_ENGINE_MATCHING_ENGINE_H

#include <atomic>
#include <iostream>

#include "order_book.h"

namespace mukhi::matching_engine {

/*
Reads orders from the provided input stream, tries to match them with exiting
orders and/or add them to the order book.

An object of this class keeps references to the streams provided during
construction time and expects them to stay alive for the lifetime of the object.

This class is thread-safe. If multiple threads try to call `Start` only one will
be successful.
*/
class MatchingEngine {
 public:
  MatchingEngine(std::istream& is, std::ostream& os, std::ostream& es)
      : is_(is), os_(os), es_(es), ob_(os_, es_) {}

  /**
  Starts the matching engine by reading from `is` and publishing trade
  events and fulfiments to `os`, and errors to `es`.

  Once the function returns after processing EOF on `is_`, it can't be
  restarted.

  This is a blocking call.
  */
  void Start();

 private:
  std::istream& is_;
  std::ostream& os_;
  std::ostream& es_;

  OrderBook ob_;

  std::atomic_bool started_ = false;
};

}  // namespace mukhi::matching_engine

#endif  // MATCHING_ENGINE_MATCHING_ENGINE_H