#include "matching_engine.h"

namespace mukhi::matching_engine {

void MatchingEngine::Start() {
    std::string line;
    while (std::getline(is_, line)) {
        auto req = parse(line, es_);
        if (req.has_value()) {
            std::visit([this](auto&& arg) {ob_.ProcessOrder(arg);}, *req);
        }
    }
}

}  // mukhi::matching_engine