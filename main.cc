#include <iostream>

#include "matching_engine.h"

int main(int argc, char** argv) {
    mukhi::matching_engine::MatchingEngine me(std::cin, std::cout, std::cerr);
    std::cout << "Starting matching engine..." << std::endl;
    me.Start();

    return 0;
}