#ifndef MATCHING_ENGINE_MATCHING_ENGINE_H
#define MATCHING_ENGINE_MATCHING_ENGINE_H

#include <iostream>

#include "order_book.h"

#ifdef UNIT_TEST
#include <atomic>
#endif  // UNIT_TEST

namespace mukhi::matching_engine {

// todo: add documentation and mention thread safety here an in all other classes.
class MatchingEngine {
public:
    MatchingEngine(std::istream& is, std::ostream& os, std::ostream& es) : is_(is), os_(os), es_(es), ob_(os_, es_) {}

    // Starts the matching engine by reading form `is` and publishing trade
    // events and fulfiments to `os`, and errors to `es`.
    // 
    // Note that this is a blocking call.
    void Start();

private:
    std::istream& is_;
    std::ostream& os_;
    std::ostream& es_;

    OrderBook ob_;

#ifdef UNIT_TEST
    std::atomic_bool stop_ = false;
    friend class MatchingEngineTest;
#endif  // UNIT_TEST
};

}  // namespace mukhi::matching_engine

#endif  // MATCHING_ENGINE_MATCHING_ENGINE_H