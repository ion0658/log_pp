#include <format>
#include <source_location>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "log.hpp"

TEST(log_pp, macro_char) {
    log_pp::set_max_level(log_pp::LevelFilter::Trace);
    LOG_PP_TRACE("This is a trace message without logger");
    LOG_PP_TRACE("This is a trace message without logger.{}", 1);
    LOG_PP_TRACE(log_pp::logger(), "This is a trace message without logger.");
    LOG_PP_TRACE(log_pp::logger(), "This is a trace message without logger.{}",
                 1);
    LOG_PP_TRACE(log_pp::logger(), {"tgt"},
                 "This is a trace message without logger.");
    LOG_PP_TRACE(log_pp::logger(), {"tgt"},
                 "This is a trace message without logger.{}", 1);
    LOG_PP_TRACE(log_pp::logger(), {"tgt"},
                 "This is a trace message without logger.{}{}", 1, 2)

    LOG_PP_DEBUG("This is a debug message without logger.{}{}", 2, 3);

    LOG_PP_DEBUG(log_pp::logger(), "This is a debug message without logger.");

    LOG_PP_INFO("This is an info message without logger.{}", 3);

    LOG_PP_INFO(log_pp::logger(), "This is an info message without logger.{}",
                1);

    LOG_PP_INFO(log_pp::logger(),
                "This is an info message without logger.{},{}", 1, 2);

    LOG_PP_INFO(log_pp::logger(), {"tgt"},
                "This is an info message without logger.{},{}", 1, 2);

    LOG_PP_INFO(log_pp::logger(), {{"key", 1}, {"name", "alice"}}, "kv {}", 7);
    LOG_PP_INFO(log_pp::logger(), {"target"}, {{"key", 1}, {"name", "alice"}},
                "kv {}", 7);
    LOG_PP_INFO({{"key", 1}, {"name", "alice"}}, "kv {}", 7);
    LOG_PP_INFO({{"tgt", 1}}, {{"ok", true}, {"ratio", 2.5}}, "target kv {}",
                "ok");
    LOG_PP_INFO({{"fmt", 1}},
                {{"hex", 255, "0x{:04X}"}, {"fixed", 1.2345, "{:.2f}"}},
                "custom {}", "kv");
}

TEST(log_pp, macro_wide) {
    log_pp::set_max_level(log_pp::LevelFilter::Trace);
    LOG_PP_TRACE(L"This is a trace message without logger");
    LOG_PP_TRACE(L"This is a trace message without logger.{}", 1);
    LOG_PP_TRACE(log_pp::logger<wchar_t>(),
                 L"This is a trace message without logger.");
    LOG_PP_TRACE(log_pp::logger<wchar_t>(),
                 L"This is a trace message without logger.{}", 1);
    LOG_PP_TRACE(log_pp::logger<wchar_t>(), {L"tgt"},
                 L"This is a trace message without logger.");
    LOG_PP_TRACE(log_pp::logger<wchar_t>(), {L"tgt"},
                 L"This is a trace message without logger.{}", 1);
    LOG_PP_TRACE(log_pp::logger<wchar_t>(), {L"tgt"},
                 L"This is a trace message without logger.{}{}", 1, 2)

    LOG_PP_DEBUG(L"This is a debug message without logger.{}{}", 2, 3);

    LOG_PP_DEBUG(log_pp::logger<wchar_t>(),
                 L"This is a debug message without logger.");

    LOG_PP_INFO(L"This is an info message without logger.{}", 3);

    LOG_PP_INFO(log_pp::logger<wchar_t>(),
                L"This is an info message without logger.{}", 1);

    LOG_PP_INFO(log_pp::logger<wchar_t>(),
                L"This is an info message without logger.{},{}", 1, 2);

    LOG_PP_INFO(log_pp::logger<wchar_t>(), {L"tgt"},
                L"This is an info message without logger.{},{}", 1, 2);

    LOG_PP_INFO(log_pp::logger<wchar_t>(), {{L"key", 1}, {L"name", L"alice"}},
                L"kv {}", 7);
    LOG_PP_INFO(log_pp::logger<wchar_t>(), {L"tgt"},
                {{L"key", 1}, {L"name", L"alice"}}, L"wide target kv {}",
                L"ok");
    LOG_PP_INFO({{L"key", 1}, {L"name", L"alice"}}, L"wide kv {}", 7);
    LOG_PP_INFO({L"tgt"}, {{L"ok", true}, {L"ratio", 2.5}},
                L"wide target kv {}", L"ok");
    LOG_PP_INFO({L"fmt"},
                {{L"hex", 255, L"0x{:04X}"}, {L"fixed", 1.2345, L"{:.2f}"}},
                L"wide custom {}", L"kv");
}

struct SimpleLogger : public log_pp::BasicLogger<char> {
    virtual bool enabled(
        const log_pp::BasicMetadata<char>& meta) const noexcept {
        std::cout << std::format("Enabled check: {} - {}\n", meta.get_level(),
                                 meta.get_target())
                  << std::endl;
        return true;
    }

    virtual void log(const log_pp::BasicRecord<char>& record) noexcept {
        auto level = std::format("[{}]", record.get_level());
        auto target = std::format("[{}]", record.get_target());
        auto kvs = [&]() {
            if (record.get_kvs().empty()) {
                return std::string("[]");
            }
            auto result = std::string("[");
            for (std::size_t i = 0; i < record.get_kvs().size(); ++i) {
                if (i != 0) {
                    result += ",";
                }
                result += std::format("{}={}", record.get_kvs()[i].get_key(),
                                      record.get_kvs()[i].get_value());
            }
            result += "]";
            return result;
        }();
        auto module = [&]() {
            if (record.get_module_path().has_value()) {
                return std::format("[{}]", record.get_module_path().value());
            } else {
                return std::string();
            }
        }();
        auto file_line = [&]() {
            if (record.get_file().has_value() &&
                record.get_line().has_value()) {
                return std::format("[{}:{}]", record.get_file().value(),
                                   record.get_line().value());
            } else {
                return std::string();
            }
        }();
        auto message =
            std::vformat(record.get_format_string(), record.get_args());
        std::cout << std::format("{} {} {} {} {} {}", level, target, kvs,
                                 module, file_line, message)
                  << std::endl;
    }

    virtual void flush() noexcept { std::cout.flush(); }
};

TEST(log_pp, macro_with_logger) {
    static SimpleLogger logger{};
    EXPECT_TRUE(log_pp::set_logger(logger));
    log_pp::set_max_level(log_pp::LevelFilter::Debug);
    testing::internal::CaptureStdout();

    LOG_PP_DEBUG({"a"}, "This is a trace message with logger.{}", 1);
    auto current_info = std::source_location::current();

    std::string out = testing::internal::GetCapturedStdout();
    EXPECT_THAT(out, testing::HasSubstr("[DEBUG] [a] []"));
    EXPECT_THAT(out,
                testing::HasSubstr("This is a trace message with logger.1"));

    testing::internal::CaptureStdout();
    LOG_PP_DEBUG({"a"}, "This is a trace message with logger.{}{}", 1, 2);
    current_info = std::source_location::current();
    out = testing::internal::GetCapturedStdout();
    EXPECT_THAT(out, testing::HasSubstr("[DEBUG] [a] []"));
    EXPECT_THAT(out,
                testing::HasSubstr("This is a trace message with logger.12"));

    testing::internal::CaptureStdout();
    LOG_PP_INFO({{"request_id", 42}, {"user", "alice"}, {"ratio", 1.5}},
                "kv with format {}", 10);
    current_info = std::source_location::current();
    out = testing::internal::GetCapturedStdout();
    EXPECT_THAT(out, testing::HasSubstr(
                         "[INFO] [] [request_id=42,user=alice,ratio=1.5]"));
    EXPECT_THAT(out, testing::HasSubstr("kv with format 10"));

    testing::internal::CaptureStdout();
    LOG_PP_INFO({"api"}, {{"count", 2}, {"status", "ok"}, {"elapsed_ms", 12.3}},
                "target + kv {}", "works");
    current_info = std::source_location::current();
    out = testing::internal::GetCapturedStdout();
    EXPECT_THAT(out, testing::HasSubstr(
                         "[INFO] [api] [count=2,status=ok,elapsed_ms=12.3]"));
    EXPECT_THAT(out, testing::HasSubstr("target + kv works"));

    testing::internal::CaptureStdout();
    LOG_PP_TRACE("This is a trace message with logger.{} will be no output", 1);
    out = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(out.empty());
}
