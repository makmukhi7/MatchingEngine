#include "matching_engine.h"

namespace mukhi::matching_engine {

void MatchingEngine::Start() {
    std::string line;
    while (true) {
        std::getline(is_, line);
        if (!line.empty()) {
            auto req = parse(line, es_);
            if (req.has_value()) {
                std::visit([this](auto&& arg) {ob_.ProcessOrder(arg);}, *req);
            }
        }

#ifdef UNIT_TEST
        if (stop_.load()) break;
#endif  // UNIT_TEST
    }
}

}  // mukhi::matching_engine