#include <format>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

#include "log.hpp"

namespace {

struct CountingValue {
    int value{};
};

inline int g_counting_value_format_calls = 0;

template <typename CharT>
struct CaptureLogger;

template <>
struct CaptureLogger<char> : public log_pp::BasicLogger<char> {
    std::string last_target{};
    std::string last_message{};
    std::string last_kv_dump{};

    bool enabled(const log_pp::BasicMetadata<char>&) const noexcept override {
        return true;
    }

    void log(const log_pp::BasicRecord<char>& record) override {
        last_target = std::string(record.get_target());
        last_message =
            std::vformat(record.get_format_string(), record.get_args());

        last_kv_dump.clear();
        for (std::size_t i = 0; i < record.get_kvs().size(); ++i) {
            if (i != 0) {
                last_kv_dump += ";";
            }
            last_kv_dump += std::format("{}={}", record.get_kvs()[i].get_key(),
                                        record.get_kvs()[i].get_value());
        }
    }

    void flush() override {}
};

static CaptureLogger<char> macro_logger;

}  // namespace

template <>
struct std::formatter<CountingValue, char> {
    std::formatter<int, char> int_formatter;

    constexpr auto parse(std::format_parse_context& ctx) {
        return int_formatter.parse(ctx);
    }

    template <typename FormatContext>
    auto format(const CountingValue& value, FormatContext& ctx) const {
        ++g_counting_value_format_calls;
        return int_formatter.format(value.value, ctx);
    }
};

TEST(log_pp_kv, format_string_and_kv_without_target) {
    static CaptureLogger<char> logger;

    log_pp::log(log_pp::Level::Info, std::source_location::current(), logger,
                {{"id", 100}, {"user", "alice"}, {"ratio", 1.25}},
                "message {} {}", "ok", 7);

    EXPECT_EQ("", logger.last_target);
    EXPECT_EQ("message ok 7", logger.last_message);
    EXPECT_EQ("id=100;user=alice;ratio=1.25", logger.last_kv_dump);
}

TEST(log_pp_kv, format_string_and_kv_with_target) {
    static CaptureLogger<char> logger;

    log_pp::log(log_pp::Level::Debug, std::source_location::current(), logger,
                {"service"}, {{"status", "retry"}, {"attempt", 2}},
                "target {}, value {}", "upload", 3.5);

    EXPECT_EQ("service", logger.last_target);
    EXPECT_EQ("target upload, value 3.5", logger.last_message);
    EXPECT_EQ("status=retry;attempt=2", logger.last_kv_dump);
}

TEST(log_pp_kv, macro_with_kv_and_format_string) {
    EXPECT_TRUE(log_pp::set_logger(macro_logger));
    log_pp::set_max_level(log_pp::LevelFilter::Trace);

    LOG_PP_INFO({"macro"}, {{"ok", true}, {"elapsed_ms", 12.0}}, "macro {} {}",
                "works", 9);

    EXPECT_EQ("macro", macro_logger.last_target);
    EXPECT_EQ("macro works 9", macro_logger.last_message);
    EXPECT_EQ("ok=true;elapsed_ms=12", macro_logger.last_kv_dump);
}

TEST(log_pp_kv, kv_value_with_custom_format) {
    static CaptureLogger<char> logger;

    log_pp::log(log_pp::Level::Info, std::source_location::current(), logger,
                {{"id", 255, "{:04X}"},
                 {"ratio", 1.236, "{:.2f}"},
                 {"scaled", 12.5, "{:.1f}"}},
                "formatted {}", "kv");

    EXPECT_EQ("", logger.last_target);
    EXPECT_EQ("formatted kv", logger.last_message);
    EXPECT_EQ("id=00FF;ratio=1.24;scaled=12.5", logger.last_kv_dump);
}

TEST(log_pp_kv, macro_with_kv_value_custom_format) {
    EXPECT_TRUE(log_pp::set_logger(macro_logger));
    log_pp::set_max_level(log_pp::LevelFilter::Trace);

    LOG_PP_INFO({"macro_fmt"}, {{"hex", 26, "0x{:X}"}, {"pad", 7, "{:03}"}},
                "custom {}", "format");

    EXPECT_EQ("macro_fmt", macro_logger.last_target);
    EXPECT_EQ("custom format", macro_logger.last_message);
    EXPECT_EQ("hex=0x1A;pad=007", macro_logger.last_kv_dump);
}

TEST(log_pp_kv, mixed_default_and_custom_kv_formatting) {
    static CaptureLogger<char> logger;

    log_pp::log(
        log_pp::Level::Info, std::source_location::current(), logger,
        {{"plain", 5}, {"fixed", 3.14159, "{:.3f}"}, {"hex", 42, "{:X}"}},
        "mix {}", "ok");

    EXPECT_EQ("mix ok", logger.last_message);
    EXPECT_EQ("plain=5;fixed=3.142;hex=2A", logger.last_kv_dump);
}

TEST(log_pp_kv, basic_kv_direct_construction_with_format_view) {
    constexpr std::string_view fmt = "0x{:X}";
    log_pp::KV kv{"id", 26, fmt};

    EXPECT_EQ("id", kv.get_key());
    EXPECT_EQ("0x1A", kv.get_value());
}

TEST(log_pp_kv, kv_value_format_is_not_evaluated_when_log_is_filtered_out) {
    static CaptureLogger<char> logger;
    g_counting_value_format_calls = 0;

    log_pp::set_max_level(log_pp::LevelFilter::Error);
    log_pp::log(log_pp::Level::Info, std::source_location::current(), logger,
                {{"counted", CountingValue{31}, "0x{:X}"}}, "filtered {}",
                "out");

    EXPECT_EQ(0, g_counting_value_format_calls);
    log_pp::set_max_level(log_pp::LevelFilter::Trace);
}

TEST(log_pp_kv, kv_keeps_lvalue_value_by_reference_when_possible) {
    std::string user = "alice";
    log_pp::KV kv{"user", user};

    user = "bob";
    EXPECT_EQ("bob", kv.get_value());
}

TEST(log_pp_kv, kv_keeps_rvalue_value_by_ownership) {
    auto kv = log_pp::KV{"user", std::string{"alice"}};

    EXPECT_EQ("alice", kv.get_value());
}
