#include "matching_engine.h"

#include <atomic>

namespace mukhi::matching_engine {

void MatchingEngine::Start() {
  bool expected = false;
  if (!started_.compare_exchange_strong(expected, true)) {
    es_ << "Matching Engine was already started" << std::endl;
    return;
  }

  std::string line;
  while (std::getline(is_, line)) {
    auto req = parse(line, es_);
    if (req.has_value()) {
      std::visit([this](auto&& arg) { ob_.ProcessOrder(arg); }, *req);
    }
  }
}

}  // namespace mukhi::matching_engine