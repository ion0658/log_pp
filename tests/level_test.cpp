#include <format>

#include <gtest/gtest.h>

#include "level.hpp"

TEST(log_pp, level_eq) {
    EXPECT_EQ(log_pp::Level::Error, log_pp::Level::Error);
    EXPECT_EQ(log_pp::Level::Error, 1);
    EXPECT_EQ(1, log_pp::Level::Error);
    EXPECT_EQ(log_pp::Level::Warning, log_pp::Level::Warning);
    EXPECT_EQ(log_pp::Level::Warning, 2);
    EXPECT_EQ(2, log_pp::Level::Warning);
    EXPECT_EQ(log_pp::Level::Info, log_pp::Level::Info);
    EXPECT_EQ(log_pp::Level::Info, 3);
    EXPECT_EQ(3, log_pp::Level::Info);
    EXPECT_EQ(log_pp::Level::Debug, log_pp::Level::Debug);
    EXPECT_EQ(log_pp::Level::Debug, 4);
    EXPECT_EQ(4, log_pp::Level::Debug);
    EXPECT_EQ(log_pp::Level::Trace, log_pp::Level::Trace);
    EXPECT_EQ(log_pp::Level::Trace, 5);
    EXPECT_EQ(5, log_pp::Level::Trace);
}

TEST(log_pp, level_neq) {
    EXPECT_NE(log_pp::Level::Error, log_pp::Level::Warning);
    EXPECT_NE(log_pp::Level::Error, 2);
    EXPECT_NE(2, log_pp::Level::Error);
    EXPECT_NE(log_pp::Level::Warning, log_pp::Level::Info);
    EXPECT_NE(log_pp::Level::Warning, 3);
    EXPECT_NE(3, log_pp::Level::Warning);
    EXPECT_NE(log_pp::Level::Info, log_pp::Level::Debug);
    EXPECT_NE(log_pp::Level::Info, 4);
    EXPECT_NE(4, log_pp::Level::Info);
    EXPECT_NE(log_pp::Level::Debug, log_pp::Level::Trace);
    EXPECT_NE(log_pp::Level::Debug, 5);
    EXPECT_NE(5, log_pp::Level::Debug);
}

TEST(log_pp, level_lt) {
    EXPECT_LT(log_pp::Level::Error, log_pp::Level::Warning);
    EXPECT_LT(log_pp::Level::Error, 2);
    EXPECT_LT(1, log_pp::Level::Warning);
    EXPECT_LT(log_pp::Level::Warning, log_pp::Level::Info);
    EXPECT_LT(log_pp::Level::Warning, 3);
    EXPECT_LT(2, log_pp::Level::Info);
    EXPECT_LT(log_pp::Level::Info, log_pp::Level::Debug);
    EXPECT_LT(log_pp::Level::Info, 4);
    EXPECT_LT(3, log_pp::Level::Debug);
    EXPECT_LT(log_pp::Level::Debug, log_pp::Level::Trace);
    EXPECT_LT(log_pp::Level::Debug, 5);
    EXPECT_LT(4, log_pp::Level::Trace);
}

TEST(log_pp, level_gt) {
    EXPECT_GT(log_pp::Level::Warning, log_pp::Level::Error);
    EXPECT_GT(log_pp::Level::Warning, 1);
    EXPECT_GT(2, log_pp::Level::Error);
    EXPECT_GT(log_pp::Level::Info, log_pp::Level::Warning);
    EXPECT_GT(log_pp::Level::Info, 2);
    EXPECT_GT(3, log_pp::Level::Warning);
    EXPECT_GT(log_pp::Level::Debug, log_pp::Level::Info);
    EXPECT_GT(log_pp::Level::Debug, 3);
    EXPECT_GT(4, log_pp::Level::Info);
    EXPECT_GT(log_pp::Level::Trace, log_pp::Level::Debug);
    EXPECT_GT(log_pp::Level::Trace, 4);
    EXPECT_GT(5, log_pp::Level::Debug);
}

TEST(log_pp, level_le) {
    EXPECT_LE(log_pp::Level::Error, log_pp::Level::Warning);
    EXPECT_LE(log_pp::Level::Error, 2);
    EXPECT_LE(1, log_pp::Level::Warning);
    EXPECT_LE(log_pp::Level::Warning, log_pp::Level::Info);
    EXPECT_LE(log_pp::Level::Warning, 3);
    EXPECT_LE(2, log_pp::Level::Info);
    EXPECT_LE(log_pp::Level::Info, log_pp::Level::Debug);
    EXPECT_LE(log_pp::Level::Info, 4);
    EXPECT_LE(3, log_pp::Level::Debug);
    EXPECT_LE(log_pp::Level::Debug, log_pp::Level::Trace);
    EXPECT_LE(log_pp::Level::Debug, 5);
    EXPECT_LE(4, log_pp::Level::Trace);
}

TEST(log_pp, level_ge) {
    EXPECT_GE(log_pp::Level::Warning, log_pp::Level::Error);
    EXPECT_GE(log_pp::Level::Warning, 1);
    EXPECT_GE(2, log_pp::Level::Error);
    EXPECT_GE(log_pp::Level::Info, log_pp::Level::Warning);
    EXPECT_GE(log_pp::Level::Info, 2);
    EXPECT_GE(3, log_pp::Level::Warning);
    EXPECT_GE(log_pp::Level::Debug, log_pp::Level::Info);
    EXPECT_GE(log_pp::Level::Debug, 3);
    EXPECT_GE(4, log_pp::Level::Info);
    EXPECT_GE(log_pp::Level::Trace, log_pp::Level::Debug);
    EXPECT_GE(log_pp::Level::Trace, 4);
    EXPECT_GE(5, log_pp::Level::Debug);
}

TEST(log_pp, level_increment) {
    log_pp::Level level = log_pp::Level::Error;
    EXPECT_EQ(level, log_pp::Level::Error);
    level++;
    EXPECT_EQ(level, log_pp::Level::Warning);
    level = level++;
    EXPECT_EQ(level, log_pp::Level::Info);
    level += 1;
    EXPECT_EQ(level, log_pp::Level::Debug);
    level++;
    EXPECT_EQ(level, log_pp::Level::Trace);
    level++;
    EXPECT_EQ(level, log_pp::Level::Trace);
    level = log_pp::Level::Debug;
    level += 10;
    EXPECT_EQ(level, log_pp::Level::Trace);
}

TEST(log_pp, level_decrement) {
    log_pp::Level level = log_pp::Level::Trace;
    EXPECT_EQ(level, log_pp::Level::Trace);
    level--;
    EXPECT_EQ(level, log_pp::Level::Debug);
    level = level--;
    EXPECT_EQ(level, log_pp::Level::Info);
    level -= 1;
    EXPECT_EQ(level, log_pp::Level::Warning);
    level--;
    EXPECT_EQ(level, log_pp::Level::Error);
    level--;
    EXPECT_EQ(level, log_pp::Level::Error);
    level = log_pp::Level::Warning;
    level -= 10;
    EXPECT_EQ(level, log_pp::Level::Error);
}

TEST(log_pp, level_format) {
    EXPECT_EQ(std::format("{}", log_pp::Level::Error), "ERROR");
    EXPECT_EQ(std::format("{}", log_pp::Level::Warning), "WARNING");
    EXPECT_EQ(std::format("{}", log_pp::Level::Info), "INFO");
    EXPECT_EQ(std::format("{}", log_pp::Level::Debug), "DEBUG");
    EXPECT_EQ(std::format("{}", log_pp::Level::Trace), "TRACE");
}
