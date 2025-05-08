#include "matching_engine.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <sstream>
#include <thread>

namespace mukhi::matching_engine {

namespace fs = std::filesystem;

TEST(MatchingEngineTest, EndToEnd) {
    for (const auto& entry : fs::directory_iterator("testdata")) {
        std::cout << "Running scenario under: " << entry.path() << std::endl;
        fs::path input_file_path = entry.path() / fs::path("input.txt");
        fs::path expected_output_file_path = entry.path() / fs::path("expected_out.txt");
        fs::path expected_error_file_path = entry.path() / fs::path("expected_err.txt");
        std::ifstream input_file(input_file_path);
        if (!input_file.is_open()) {
            std::cerr << "Couldn't open input file." << std::endl;
            continue;
        }
        std::ifstream expected_output_file(expected_output_file_path);
        if (!expected_output_file.is_open()) {
            std::cerr << "Couldn't open expected output file." << std::endl;
            continue;
        }
        std::ifstream expected_error_file(expected_error_file_path);
        if (!expected_error_file.is_open()) {
            std::cerr << "Couldn't open expected error file." << std::endl;
            continue;
        }

        // Read input from test scenario.
        std::string line;
        std::string input;
        while (std::getline(input_file, line)) {
            // Ignore everything after space
            if (size_t pos = line.find(" "); pos != std::string::npos) {
                line = line.substr(0, pos);
            }
            input += line + "\n";
        }

        // Start Matching Engine
        std::istringstream is(input);
        std::ostringstream os;
        std::ostringstream es;
        MatchingEngine me(is, os, es);
        std::thread t(&MatchingEngine::Start, &me);

        // Prepare expectations.
        std::string expected_output;
        while (std::getline(expected_output_file, line)) {
            // Ignore everything after space
            if (size_t pos = line.find(" "); pos != std::string::npos) {
                line = line.substr(0, pos);
            }
            expected_output += line + "\n";
        }
        std::string expected_error;
        while (std::getline(expected_error_file, line)) {
            expected_error += line + "\n";
        }

        // Give matching engine plenty of time before stopping.
        std::this_thread::sleep_for(std::chrono::seconds(1));
        // Stop matching engine by setting EOF on input stream.
        is.setstate(std::ios_base::eofbit);
        t.join();

        // Make assertions.
        EXPECT_THAT(os.str(), expected_output);
        EXPECT_THAT(es.str(), expected_error);
    }
}

}  // namespace mukhi::matching_engine