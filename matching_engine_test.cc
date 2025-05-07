#define UNIT_TEST
#include "matching_engine.h"

#include <chrono>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <sstream>
#include <thread>

namespace mukhi::matching_engine {

class MatchingEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        me_ = std::make_unique<MatchingEngine>(is_, os_, es_);
    }

    void Stop() {
        me_->stop_.store(true);
    }

    std::istringstream is_;
    std::ostringstream os_;
    std::ostringstream es_;
    std::unique_ptr<MatchingEngine> me_;
};

TEST_F(MatchingEngineTest, Simple) {
    std::thread t(&MatchingEngine::Start, me_.get());
    std::string input;
    input += "0,1000000,1,1,1075\n";
    input += "0,1000001,0,9,1000\n";
    input += "0,1000002,0,30,975\n";
    input += "0,1000003,1,10,1050\n";
    input += "0,1000004,0,10,950\n";
    input += "BADMESSAGE\n";
    input += "0,1000005,1,2,1025\n";
    input += "0,1000006,0,1,1000\n";
    input += "1,1000004\n";
    input += "0,1000007,1,5,1025\n";
    input += "0,1000008,0,3,1050\n";

    is_.str(input);

    std::ostringstream expected;
    expected << "2,2,1025" << std::endl;
    expected << "4,1000008,1" << std::endl;
    expected << "3,1000005" << std::endl;
    expected << "2,1,1025" << std::endl;
    expected << "3,1000008" << std::endl;
    expected << "4,1000007,4" << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    Stop();
    t.join();

    EXPECT_THAT(os_.str(), expected.str());
    EXPECT_THAT(es_.str(), ::testing::HasSubstr("Bad message"));

}

}  // namespace mukhi::matching_engine