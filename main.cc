#include <iostream>

#include "messages.h"

int main(int argc, char** argv) {
    mukhi::matching_engine::TradeEvent te{ .qty = 10, .price = 75 };
    std::cout << te << std::endl;
    return 0;
}