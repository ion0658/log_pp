#include <format>

#include <gtest/gtest.h>

#include "log.hpp"

namespace {

struct CaptureLogger : public log_pp::BasicLogger<char> {
    std::string last_message;
    log_pp::Level last_level = log_pp::Level::Error;

    bool enabled(const log_pp::BasicMetadata<char>&) const noexcept override {
        return true;
    }

    void log(const log_pp::BasicRecord<char>& record) noexcept override {
        last_level = record.get_level();
        last_message =
            std::vformat(record.get_format_string(), record.get_args());
    }

    void flush() noexcept override {}
};

CaptureLogger shared_logger;

}  // namespace

TEST(log_pp, fallback_release) {
    EXPECT_TRUE(log_pp::set_logger(shared_logger) ||
                &log_pp::logger() == &shared_logger);
    log_pp::set_max_level(log_pp::LevelFilter::Trace);

    LOG_PP_DEBUG("debug {}", 1);
    EXPECT_EQ(shared_logger.last_level, log_pp::Level::Error);
    EXPECT_EQ(shared_logger.last_message, "");

    LOG_PP_TRACE("trace {}", 2);
    EXPECT_EQ(shared_logger.last_level, log_pp::Level::Error);
    EXPECT_EQ(shared_logger.last_message, "");
}

TEST(log_pp, fallback_release_info) {
    EXPECT_TRUE(log_pp::set_logger(shared_logger) ||
                &log_pp::logger() == &shared_logger);
    log_pp::set_max_level(log_pp::LevelFilter::Trace);

    LOG_PP_INFO("info {}", 1);

    EXPECT_EQ(shared_logger.last_level, log_pp::Level::Info);
    EXPECT_EQ(shared_logger.last_message, "info 1");
}
