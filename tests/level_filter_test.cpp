#include <format>

#include <gtest/gtest.h>

#include "level.hpp"

TEST(log_pp, level_filter_eq) {
    EXPECT_EQ(log_pp::LevelFilter::Off, log_pp::LevelFilter::Off);

    EXPECT_EQ(log_pp::LevelFilter::Error, log_pp::LevelFilter::Error);
    EXPECT_EQ(log_pp::LevelFilter::Error, log_pp::Level::Error);
    EXPECT_EQ(log_pp::Level::Error, log_pp::LevelFilter::Error);

    EXPECT_EQ(log_pp::LevelFilter::Warn, log_pp::LevelFilter::Warn);
    EXPECT_EQ(log_pp::LevelFilter::Warn, log_pp::Level::Warning);
    EXPECT_EQ(log_pp::Level::Warning, log_pp::LevelFilter::Warn);

    EXPECT_EQ(log_pp::LevelFilter::Info, log_pp::LevelFilter::Info);
    EXPECT_EQ(log_pp::LevelFilter::Info, log_pp::Level::Info);
    EXPECT_EQ(log_pp::Level::Info, log_pp::LevelFilter::Info);

    EXPECT_EQ(log_pp::LevelFilter::Debug, log_pp::LevelFilter::Debug);
    EXPECT_EQ(log_pp::LevelFilter::Debug, log_pp::Level::Debug);
    EXPECT_EQ(log_pp::Level::Debug, log_pp::LevelFilter::Debug);

    EXPECT_EQ(log_pp::LevelFilter::Trace, log_pp::LevelFilter::Trace);
    EXPECT_EQ(log_pp::LevelFilter::Trace, log_pp::Level::Trace);
    EXPECT_EQ(log_pp::Level::Trace, log_pp::LevelFilter::Trace);
}

TEST(log_pp, level_filter_neq) {
    EXPECT_NE(log_pp::LevelFilter::Off, log_pp::LevelFilter::Error);
    EXPECT_NE(log_pp::LevelFilter::Off, log_pp::Level::Error);
    EXPECT_NE(log_pp::Level::Error, log_pp::LevelFilter::Off);

    EXPECT_NE(log_pp::LevelFilter::Error, log_pp::LevelFilter::Warn);
    EXPECT_NE(log_pp::LevelFilter::Error, log_pp::Level::Warning);
    EXPECT_NE(log_pp::Level::Warning, log_pp::LevelFilter::Error);

    EXPECT_NE(log_pp::LevelFilter::Warn, log_pp::LevelFilter::Info);
    EXPECT_NE(log_pp::LevelFilter::Warn, log_pp::Level::Info);
    EXPECT_NE(log_pp::Level::Info, log_pp::LevelFilter::Warn);

    EXPECT_NE(log_pp::LevelFilter::Info, log_pp::LevelFilter::Debug);
    EXPECT_NE(log_pp::LevelFilter::Info, log_pp::Level::Debug);
    EXPECT_NE(log_pp::Level::Debug, log_pp::LevelFilter::Info);

    EXPECT_NE(log_pp::LevelFilter::Debug, log_pp::LevelFilter::Trace);
    EXPECT_NE(log_pp::LevelFilter::Debug, log_pp::Level::Trace);
    EXPECT_NE(log_pp::Level::Trace, log_pp::LevelFilter::Debug);

    EXPECT_NE(log_pp::LevelFilter::Trace, log_pp::LevelFilter::Off);
}

TEST(log_pp, level_filter_lt) {
    EXPECT_LT(log_pp::LevelFilter::Off, log_pp::LevelFilter::Error);
    EXPECT_LT(log_pp::LevelFilter::Off, log_pp::Level::Error);

    EXPECT_LT(log_pp::LevelFilter::Error, log_pp::LevelFilter::Warn);
    EXPECT_LT(log_pp::LevelFilter::Error, log_pp::Level::Warning);
    EXPECT_LT(log_pp::Level::Error, log_pp::LevelFilter::Warn);

    EXPECT_LT(log_pp::LevelFilter::Warn, log_pp::LevelFilter::Info);
    EXPECT_LT(log_pp::LevelFilter::Warn, log_pp::Level::Info);
    EXPECT_LT(log_pp::Level::Warning, log_pp::LevelFilter::Info);

    EXPECT_LT(log_pp::LevelFilter::Info, log_pp::LevelFilter::Debug);
    EXPECT_LT(log_pp::LevelFilter::Info, log_pp::Level::Debug);
    EXPECT_LT(log_pp::Level::Info, log_pp::LevelFilter::Debug);

    EXPECT_LT(log_pp::LevelFilter::Debug, log_pp::LevelFilter::Trace);
    EXPECT_LT(log_pp::LevelFilter::Debug, log_pp::Level::Trace);
    EXPECT_LT(log_pp::Level::Debug, log_pp::LevelFilter::Trace);
}

TEST(log_pp, level_filter_gt) {
    EXPECT_GT(log_pp::LevelFilter::Error, log_pp::LevelFilter::Off);
    EXPECT_GT(log_pp::Level::Error, log_pp::LevelFilter::Off);

    EXPECT_GT(log_pp::LevelFilter::Warn, log_pp::LevelFilter::Error);
    EXPECT_GT(log_pp::LevelFilter::Warn, log_pp::Level::Error);
    EXPECT_GT(log_pp::Level::Warning, log_pp::LevelFilter::Error);

    EXPECT_GT(log_pp::LevelFilter::Info, log_pp::LevelFilter::Warn);
    EXPECT_GT(log_pp::LevelFilter::Info, log_pp::LevelFilter::Warn);
    EXPECT_GT(log_pp::Level::Info, log_pp::LevelFilter::Warn);

    EXPECT_GT(log_pp::LevelFilter::Debug, log_pp::LevelFilter::Info);
    EXPECT_GT(log_pp::LevelFilter::Debug, log_pp::LevelFilter::Info);
    EXPECT_GT(log_pp::Level::Debug, log_pp::LevelFilter::Info);

    EXPECT_GT(log_pp::LevelFilter::Trace, log_pp::LevelFilter::Debug);
    EXPECT_GT(log_pp::LevelFilter::Trace, log_pp::LevelFilter::Debug);
    EXPECT_GT(log_pp::Level::Trace, log_pp::LevelFilter::Debug);
}

TEST(log_pp, level_filter_le) {
    EXPECT_LE(log_pp::LevelFilter::Error, log_pp::LevelFilter::Warn);
    EXPECT_LE(log_pp::LevelFilter::Error, log_pp::Level::Warning);
    EXPECT_LE(log_pp::Level::Error, log_pp::LevelFilter::Warn);

    EXPECT_LE(log_pp::LevelFilter::Warn, log_pp::LevelFilter::Info);
    EXPECT_LE(log_pp::LevelFilter::Warn, log_pp::Level::Info);
    EXPECT_LE(log_pp::Level::Warning, log_pp::LevelFilter::Info);

    EXPECT_LE(log_pp::LevelFilter::Info, log_pp::LevelFilter::Debug);
    EXPECT_LE(log_pp::LevelFilter::Info, log_pp::Level::Debug);
    EXPECT_LE(log_pp::Level::Info, log_pp::LevelFilter::Debug);

    EXPECT_LE(log_pp::LevelFilter::Debug, log_pp::LevelFilter::Trace);
    EXPECT_LE(log_pp::LevelFilter::Debug, log_pp::Level::Trace);
    EXPECT_LE(log_pp::Level::Debug, log_pp::LevelFilter::Trace);

    EXPECT_LE(log_pp::LevelFilter::Trace, log_pp::LevelFilter::Trace);
    EXPECT_LE(log_pp::LevelFilter::Trace, log_pp::Level::Trace);
}

TEST(log_pp, level_filter_ge) {
    EXPECT_GE(log_pp::LevelFilter::Warn, log_pp::LevelFilter::Error);
    EXPECT_GE(log_pp::LevelFilter::Warn, log_pp::Level::Error);
    EXPECT_GE(log_pp::Level::Warning, log_pp::LevelFilter::Error);

    EXPECT_GE(log_pp::LevelFilter::Info, log_pp::LevelFilter::Warn);
    EXPECT_GE(log_pp::LevelFilter::Info, log_pp::Level::Warning);
    EXPECT_GE(log_pp::Level::Info, log_pp::LevelFilter::Warn);

    EXPECT_GE(log_pp::LevelFilter::Debug, log_pp::LevelFilter::Info);
    EXPECT_GE(log_pp::LevelFilter::Debug, log_pp::Level::Info);
    EXPECT_GE(log_pp::Level::Debug, log_pp::LevelFilter::Info);

    EXPECT_GE(log_pp::LevelFilter::Trace, log_pp::LevelFilter::Debug);
    EXPECT_GE(log_pp::LevelFilter::Trace, log_pp::Level::Debug);
    EXPECT_GE(log_pp::Level::Trace, log_pp::LevelFilter::Debug);

    EXPECT_GE(log_pp::LevelFilter::Trace, log_pp::LevelFilter::Trace);
    EXPECT_GE(log_pp::LevelFilter::Trace, log_pp::LevelFilter::Trace);
}

TEST(log_pp, level_filter_increment) {
    log_pp::LevelFilter level = log_pp::LevelFilter::Off;
    EXPECT_EQ(level, log_pp::LevelFilter::Off);
    level++;
    EXPECT_EQ(level, log_pp::LevelFilter::Error);
    level++;
    EXPECT_EQ(level, log_pp::LevelFilter::Warn);
    level = level++;
    EXPECT_EQ(level, log_pp::LevelFilter::Info);
    level += 1;
    EXPECT_EQ(level, log_pp::LevelFilter::Debug);
    level++;
    EXPECT_EQ(level, log_pp::LevelFilter::Trace);
    level++;
    EXPECT_EQ(level, log_pp::LevelFilter::Trace);
    level = log_pp::LevelFilter::Debug;
    level += 10;
    EXPECT_EQ(level, log_pp::LevelFilter::Trace);
}

TEST(log_pp, level_filter_decrement) {
    log_pp::LevelFilter level = log_pp::LevelFilter::Trace;
    EXPECT_EQ(level, log_pp::LevelFilter::Trace);
    level--;
    EXPECT_EQ(level, log_pp::LevelFilter::Debug);
    level = level--;
    EXPECT_EQ(level, log_pp::LevelFilter::Info);
    level -= 1;
    EXPECT_EQ(level, log_pp::LevelFilter::Warn);
    level--;
    EXPECT_EQ(level, log_pp::LevelFilter::Error);
    level--;
    EXPECT_EQ(level, log_pp::LevelFilter::Off);
    level--;
    EXPECT_EQ(level, log_pp::LevelFilter::Off);
    level = log_pp::LevelFilter::Warn;
    level -= 10;
    EXPECT_EQ(level, log_pp::LevelFilter::Off);
}

TEST(log_pp, level_filter_format) {
    EXPECT_EQ(std::format("{}", log_pp::LevelFilter::Off), "UNKNOWN");
    EXPECT_EQ(std::format("{}", log_pp::LevelFilter::Error), "ERROR");
    EXPECT_EQ(std::format("{}", log_pp::LevelFilter::Warn), "WARNING");
    EXPECT_EQ(std::format("{}", log_pp::LevelFilter::Info), "INFO");
    EXPECT_EQ(std::format("{}", log_pp::LevelFilter::Debug), "DEBUG");
    EXPECT_EQ(std::format("{}", log_pp::LevelFilter::Trace), "TRACE");
}
