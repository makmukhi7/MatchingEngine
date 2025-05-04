#include <iostream>

#include "messages.h"

int main(int argc, char** argv) {
    mukhi::matching_engine::TradeEvent te{ .price = 75, .qty = 10 };
    std::cout << te << std::endl;
    return 0;
}