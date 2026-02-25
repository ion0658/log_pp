#include <format>
#include <initializer_list>
#include <string>

#include <gtest/gtest.h>

#include "kv.hpp"
#include "log.hpp"

namespace {

template <typename CharT>
struct MacroCaptureLogger;

template <>
struct MacroCaptureLogger<char> : public log_pp::BasicLogger<char> {
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
            last_kv_dump +=
                std::format("{}={}", record.get_kvs()[i].get_key_str(),
                            record.get_kvs()[i].get_value_string());
        }
    }

    void flush() override {}

    void clear() {
        last_target.clear();
        last_message.clear();
        last_kv_dump.clear();
    }
};

template <>
struct MacroCaptureLogger<wchar_t> : public log_pp::BasicLogger<wchar_t> {
    std::wstring last_target{};
    std::wstring last_message{};
    std::wstring last_kv_dump{};

    bool enabled(
        const log_pp::BasicMetadata<wchar_t>&) const noexcept override {
        return true;
    }

    void log(const log_pp::BasicRecord<wchar_t>& record) override {
        last_target = std::wstring(record.get_target());
        last_message =
            std::vformat(record.get_format_string(), record.get_args());

        last_kv_dump.clear();
        for (std::size_t i = 0; i < record.get_kvs().size(); ++i) {
            if (i != 0) {
                last_kv_dump += L";";
            }
            last_kv_dump +=
                std::format(L"{}={}", record.get_kvs()[i].get_key_str(),
                            record.get_kvs()[i].get_value_string());
        }
    }

    void flush() override {}

    void clear() {
        last_target.clear();
        last_message.clear();
        last_kv_dump.clear();
    }
};

static MacroCaptureLogger<char> g_global_char_logger;
static MacroCaptureLogger<wchar_t> g_global_wchar_logger;

}  // namespace

// multi byte character test

TEST(log_pp, macro_basic) {
    static MacroCaptureLogger<char> logger;
    logger.clear();
    log_pp::set_max_level(log_pp::LevelFilter::Trace);

    LOG_PP_TRACE(logger, {"tgt"},
                 {
                     {"k", "v"},
                     {"k2", 1},
                     {"k3", true},
                     {"k4", 1.1},
                     {"format_val", 100, "0x{:02X}"},
                 },
                 "trace {}", 1);
    EXPECT_EQ("tgt", logger.last_target);
    EXPECT_EQ("trace 1", logger.last_message);
    EXPECT_EQ("k=v;k2=1;k3=true;k4=1.1;format_val=0x64", logger.last_kv_dump);

    LOG_PP_TRACE(logger, {"tgt"},
                 {
                     {"k", "v"},
                     {"k2", 1},
                     {"k3", true},
                     {"k4", 1.1},
                     {"format_val", 100, "0x{:02X}"},
                 },
                 "message only");
    EXPECT_EQ("tgt", logger.last_target);
    EXPECT_EQ("message only", logger.last_message);
    EXPECT_EQ("k=v;k2=1;k3=true;k4=1.1;format_val=0x64", logger.last_kv_dump);
}

TEST(log_pp, macro_expect_target) {
    static MacroCaptureLogger<char> logger;
    logger.clear();
    log_pp::set_max_level(log_pp::LevelFilter::Trace);

    LOG_PP_TRACE(logger,
                 {
                     {"k", "v"},
                     {"k2", 1},
                     {"k3", true},
                     {"k4", 1.1},
                     {"format_val", 100, "0x{:02X}"},
                 },
                 "message {}", 1);
    EXPECT_EQ("", logger.last_target);
    EXPECT_EQ("message 1", logger.last_message);
    EXPECT_EQ("k=v;k2=1;k3=true;k4=1.1;format_val=0x64", logger.last_kv_dump);

    LOG_PP_TRACE(logger,
                 {
                     {"k", "v"},
                     {"k2", 1},
                     {"k3", true},
                     {"k4", 1.1},
                     {"format_val", 100, "0x{:02X}"},
                 },
                 "message only");
    EXPECT_EQ("", logger.last_target);
    EXPECT_EQ("message only", logger.last_message);
    EXPECT_EQ("k=v;k2=1;k3=true;k4=1.1;format_val=0x64", logger.last_kv_dump);
}

TEST(log_pp, macro_expect_kv) {
    static MacroCaptureLogger<char> logger;
    logger.clear();
    log_pp::set_max_level(log_pp::LevelFilter::Trace);

    LOG_PP_TRACE(logger, {"tgt"}, "message {}", 1);
    EXPECT_EQ("tgt", logger.last_target);
    EXPECT_EQ("message 1", logger.last_message);
    EXPECT_EQ("", logger.last_kv_dump);

    LOG_PP_TRACE(logger, {"tgt2"}, "message only");
    EXPECT_EQ("tgt2", logger.last_target);
    EXPECT_EQ("message only", logger.last_message);
    EXPECT_EQ("", logger.last_kv_dump);
}

TEST(log_pp, macro_expect_target_and_kv) {
    static MacroCaptureLogger<char> logger;
    logger.clear();
    log_pp::set_max_level(log_pp::LevelFilter::Trace);

    LOG_PP_TRACE(logger, "message {}", 1);
    EXPECT_EQ("", logger.last_target);
    EXPECT_EQ("message 1", logger.last_message);
    EXPECT_EQ("", logger.last_kv_dump);

    LOG_PP_TRACE(logger, "message only");
    EXPECT_EQ("", logger.last_target);
    EXPECT_EQ("message only", logger.last_message);
    EXPECT_EQ("", logger.last_kv_dump);
}

TEST(log_pp, macro_expect_message) {
    static MacroCaptureLogger<char> logger;
    logger.clear();
    log_pp::set_max_level(log_pp::LevelFilter::Trace);

    LOG_PP_TRACE(logger, {"tgt"},
                 {
                     {"k", "v"},
                     {"k2", 1},
                     {"k3", true},
                     {"k4", 1.1},
                     {"format_val", 100, "0x{:02X}"},
                 });
    EXPECT_EQ("tgt", logger.last_target);
    EXPECT_EQ("", logger.last_message);
    EXPECT_EQ("k=v;k2=1;k3=true;k4=1.1;format_val=0x64", logger.last_kv_dump);

    LOG_PP_TRACE(logger, std::initializer_list<log_pp::BasicKV<char>>{
                             {"k", "v"},
                             {"k2", 1},
                             {"k3", true},
                             {"k4", 1.1},
                             {"format_val", 100, "0x{:02X}"},
                         });
    EXPECT_EQ("", logger.last_target);
    EXPECT_EQ("", logger.last_message);
    EXPECT_EQ("k=v;k2=1;k3=true;k4=1.1;format_val=0x64", logger.last_kv_dump);
}

TEST(log_pp, macro_use_global_logger_basic) {
    g_global_char_logger.clear();
    log_pp::set_max_level(log_pp::LevelFilter::Trace);
    log_pp::set_logger(g_global_char_logger);

    LOG_PP_TRACE({"tgt"},
                 {
                     {"k", "v"},
                     {"k2", 1},
                     {"k3", true},
                     {"k4", 1.1},
                     {"format_val", 100, "0x{:02X}"},
                 },
                 "trace {}", 1);
    EXPECT_EQ("tgt", g_global_char_logger.last_target);
    EXPECT_EQ("trace 1", g_global_char_logger.last_message);
    EXPECT_EQ("k=v;k2=1;k3=true;k4=1.1;format_val=0x64",
              g_global_char_logger.last_kv_dump);

    LOG_PP_TRACE({"tgt"},
                 {
                     {"k", "v"},
                     {"k2", 1},
                     {"k3", true},
                     {"k4", 1.1},
                     {"format_val", 100, "0x{:02X}"},
                 },
                 "message only");
    EXPECT_EQ("tgt", g_global_char_logger.last_target);
    EXPECT_EQ("message only", g_global_char_logger.last_message);
    EXPECT_EQ("k=v;k2=1;k3=true;k4=1.1;format_val=0x64",
              g_global_char_logger.last_kv_dump);
}

TEST(log_pp, macro_use_global_logger_expect_target) {
    g_global_char_logger.clear();
    log_pp::set_max_level(log_pp::LevelFilter::Trace);
    log_pp::set_logger(g_global_char_logger);

    LOG_PP_TRACE(
        {
            {"k", "v"},
            {"k2", 1},
            {"k3", true},
            {"k4", 1.1},
            {"format_val", 100, "0x{:02X}"},
        },
        "trace {}", 1);
    EXPECT_EQ("", g_global_char_logger.last_target);
    EXPECT_EQ("trace 1", g_global_char_logger.last_message);
    EXPECT_EQ("k=v;k2=1;k3=true;k4=1.1;format_val=0x64",
              g_global_char_logger.last_kv_dump);

    LOG_PP_TRACE(
        {
            {"k", "v"},
            {"k2", 1},
            {"k3", true},
            {"k4", 1.1},
            {"format_val", 100, "0x{:02X}"},
        },
        "message only");
    EXPECT_EQ("", g_global_char_logger.last_target);
    EXPECT_EQ("message only", g_global_char_logger.last_message);
    EXPECT_EQ("k=v;k2=1;k3=true;k4=1.1;format_val=0x64",
              g_global_char_logger.last_kv_dump);
}

TEST(log_pp, macro_use_global_logger_expect_kv) {
    g_global_char_logger.clear();
    log_pp::set_max_level(log_pp::LevelFilter::Trace);
    log_pp::set_logger(g_global_char_logger);

    LOG_PP_TRACE({"tgt"}, "trace {}", 1);
    EXPECT_EQ("tgt", g_global_char_logger.last_target);
    EXPECT_EQ("trace 1", g_global_char_logger.last_message);
    EXPECT_EQ("", g_global_char_logger.last_kv_dump);
    g_global_char_logger.clear();

    LOG_PP_TRACE({"tgt2"}, "message only");
    EXPECT_EQ("tgt2", g_global_char_logger.last_target);
    EXPECT_EQ("message only", g_global_char_logger.last_message);
    EXPECT_EQ("", g_global_char_logger.last_kv_dump);
    g_global_char_logger.clear();
}

TEST(log_pp, macro_use_global_logger_expect_target_and_kv) {
    g_global_char_logger.clear();
    log_pp::set_max_level(log_pp::LevelFilter::Trace);
    log_pp::set_logger(g_global_char_logger);

    LOG_PP_TRACE("trace {}", 1);
    EXPECT_EQ("", g_global_char_logger.last_target);
    EXPECT_EQ("trace 1", g_global_char_logger.last_message);
    EXPECT_EQ("", g_global_char_logger.last_kv_dump);
    g_global_char_logger.clear();

    LOG_PP_TRACE("message only");
    EXPECT_EQ("", g_global_char_logger.last_target);
    EXPECT_EQ("message only", g_global_char_logger.last_message);
    EXPECT_EQ("", g_global_char_logger.last_kv_dump);
    g_global_char_logger.clear();
}

TEST(log_pp, macro_wchar_basic) {
    static MacroCaptureLogger<wchar_t> logger;
    logger.clear();
    log_pp::set_max_level(log_pp::LevelFilter::Trace);

    LOG_PP_TRACE(logger, {L"tgt"},
                 {
                     {L"k", L"v"},
                     {L"k2", 1},
                     {L"k3", true},
                     {L"k4", 1.1},
                     {L"format_val", 100, L"0x{:02X}"},
                 },
                 L"trace {}", 1);
    EXPECT_EQ(L"tgt", logger.last_target);
    EXPECT_EQ(L"trace 1", logger.last_message);
    EXPECT_EQ(L"k=v;k2=1;k3=true;k4=1.1;format_val=0x64", logger.last_kv_dump);

    LOG_PP_TRACE(logger, {L"tgt"},
                 {
                     {L"k", L"v"},
                     {L"k2", 1},
                     {L"k3", true},
                     {L"k4", 1.1},
                     {L"format_val", 100, L"0x{:02X}"},
                 },
                 L"message only");
    EXPECT_EQ(L"tgt", logger.last_target);
    EXPECT_EQ(L"message only", logger.last_message);
    EXPECT_EQ(L"k=v;k2=1;k3=true;k4=1.1;format_val=0x64", logger.last_kv_dump);
}

TEST(log_pp, macro_wchar_expect_target) {
    static MacroCaptureLogger<wchar_t> logger;
    logger.clear();
    log_pp::set_max_level(log_pp::LevelFilter::Trace);

    LOG_PP_TRACE(logger,
                 {
                     {L"k", L"v"},
                     {L"k2", 1},
                     {L"k3", true},
                     {L"k4", 1.1},
                     {L"format_val", 100, L"0x{:02X}"},
                 },
                 L"message {}", 1);
    EXPECT_EQ(L"", logger.last_target);
    EXPECT_EQ(L"message 1", logger.last_message);
    EXPECT_EQ(L"k=v;k2=1;k3=true;k4=1.1;format_val=0x64", logger.last_kv_dump);

    LOG_PP_TRACE(logger,
                 {
                     {L"k", L"v"},
                     {L"k2", 1},
                     {L"k3", true},
                     {L"k4", 1.1},
                     {L"format_val", 100, L"0x{:02X}"},
                 },
                 L"message only");
    EXPECT_EQ(L"", logger.last_target);
    EXPECT_EQ(L"message only", logger.last_message);
    EXPECT_EQ(L"k=v;k2=1;k3=true;k4=1.1;format_val=0x64", logger.last_kv_dump);
}

TEST(log_pp, macro_wchar_expect_kv) {
    static MacroCaptureLogger<wchar_t> logger;
    logger.clear();
    log_pp::set_max_level(log_pp::LevelFilter::Trace);

    LOG_PP_TRACE(logger, {L"tgt"}, L"message {}", 1);
    EXPECT_EQ(L"tgt", logger.last_target);
    EXPECT_EQ(L"message 1", logger.last_message);
    EXPECT_EQ(L"", logger.last_kv_dump);

    LOG_PP_TRACE(logger, {L"tgt2"}, L"message only");
    EXPECT_EQ(L"tgt2", logger.last_target);
    EXPECT_EQ(L"message only", logger.last_message);
    EXPECT_EQ(L"", logger.last_kv_dump);
}

TEST(log_pp, macro_wchar_expect_target_and_kv) {
    static MacroCaptureLogger<wchar_t> logger;
    logger.clear();
    log_pp::set_max_level(log_pp::LevelFilter::Trace);

    LOG_PP_TRACE(logger, L"message {}", 1);
    EXPECT_EQ(L"", logger.last_target);
    EXPECT_EQ(L"message 1", logger.last_message);
    EXPECT_EQ(L"", logger.last_kv_dump);

    LOG_PP_TRACE(logger, L"message only");
    EXPECT_EQ(L"", logger.last_target);
    EXPECT_EQ(L"message only", logger.last_message);
    EXPECT_EQ(L"", logger.last_kv_dump);
}

TEST(log_pp, macro_wchar_expect_message) {
    static MacroCaptureLogger<wchar_t> logger;
    logger.clear();
    log_pp::set_max_level(log_pp::LevelFilter::Trace);

    LOG_PP_TRACE(logger, {L"tgt"},
                 {
                     {L"k", L"v"},
                     {L"k2", 1},
                     {L"k3", true},
                     {L"k4", 1.1},
                     {L"format_val", 100, L"0x{:02X}"},
                 });
    EXPECT_EQ(L"tgt", logger.last_target);
    EXPECT_EQ(L"", logger.last_message);
    EXPECT_EQ(L"k=v;k2=1;k3=true;k4=1.1;format_val=0x64", logger.last_kv_dump);

    LOG_PP_TRACE(logger, std::initializer_list<log_pp::BasicKV<wchar_t>>{
                             {L"k", L"v"},
                             {L"k2", 1},
                             {L"k3", true},
                             {L"k4", 1.1},
                             {L"format_val", 100, L"0x{:02X}"},
                         });
    EXPECT_EQ(L"", logger.last_target);
    EXPECT_EQ(L"", logger.last_message);
    EXPECT_EQ(L"k=v;k2=1;k3=true;k4=1.1;format_val=0x64", logger.last_kv_dump);
}

TEST(log_pp, macro_wchar_use_global_logger_basic) {
    g_global_wchar_logger.clear();
    log_pp::set_max_level(log_pp::LevelFilter::Trace);
    log_pp::set_logger(g_global_wchar_logger);

    LOG_PP_TRACE({L"tgt"},
                 {
                     {L"k", L"v"},
                     {L"k2", 1},
                     {L"k3", true},
                     {L"k4", 1.1},
                     {L"format_val", 100, L"0x{:02X}"},
                 },
                 L"trace {}", 1);
    EXPECT_EQ(L"tgt", g_global_wchar_logger.last_target);
    EXPECT_EQ(L"trace 1", g_global_wchar_logger.last_message);
    EXPECT_EQ(L"k=v;k2=1;k3=true;k4=1.1;format_val=0x64",
              g_global_wchar_logger.last_kv_dump);

    LOG_PP_TRACE({L"tgt"},
                 {
                     {L"k", L"v"},
                     {L"k2", 1},
                     {L"k3", true},
                     {L"k4", 1.1},
                     {L"format_val", 100, L"0x{:02X}"},
                 },
                 L"message only");
    EXPECT_EQ(L"tgt", g_global_wchar_logger.last_target);
    EXPECT_EQ(L"message only", g_global_wchar_logger.last_message);
    EXPECT_EQ(L"k=v;k2=1;k3=true;k4=1.1;format_val=0x64",
              g_global_wchar_logger.last_kv_dump);
}

TEST(log_pp, macro_wchar_use_global_logger_expect_target) {
    g_global_wchar_logger.clear();
    log_pp::set_max_level(log_pp::LevelFilter::Trace);
    log_pp::set_logger(g_global_wchar_logger);

    LOG_PP_TRACE(
        {
            {L"k", L"v"},
            {L"k2", 1},
            {L"k3", true},
            {L"k4", 1.1},
            {L"format_val", 100, L"0x{:02X}"},
        },
        L"trace {}", 1);
    EXPECT_EQ(L"", g_global_wchar_logger.last_target);
    EXPECT_EQ(L"trace 1", g_global_wchar_logger.last_message);
    EXPECT_EQ(L"k=v;k2=1;k3=true;k4=1.1;format_val=0x64",
              g_global_wchar_logger.last_kv_dump);

    LOG_PP_TRACE(
        {
            {L"k", L"v"},
            {L"k2", 1},
            {L"k3", true},
            {L"k4", 1.1},
            {L"format_val", 100, L"0x{:02X}"},
        },
        L"message only");
    EXPECT_EQ(L"", g_global_wchar_logger.last_target);
    EXPECT_EQ(L"message only", g_global_wchar_logger.last_message);
    EXPECT_EQ(L"k=v;k2=1;k3=true;k4=1.1;format_val=0x64",
              g_global_wchar_logger.last_kv_dump);
}

TEST(log_pp, macro_wchar_use_global_logger_expect_kv) {
    g_global_wchar_logger.clear();
    log_pp::set_max_level(log_pp::LevelFilter::Trace);
    log_pp::set_logger(g_global_wchar_logger);

    LOG_PP_TRACE({L"tgt"}, L"trace {}", 1);
    EXPECT_EQ(L"tgt", g_global_wchar_logger.last_target);
    EXPECT_EQ(L"trace 1", g_global_wchar_logger.last_message);
    EXPECT_EQ(L"", g_global_wchar_logger.last_kv_dump);
    g_global_wchar_logger.clear();

    LOG_PP_TRACE({L"tgt2"}, L"message only");
    EXPECT_EQ(L"tgt2", g_global_wchar_logger.last_target);
    EXPECT_EQ(L"message only", g_global_wchar_logger.last_message);
    EXPECT_EQ(L"", g_global_wchar_logger.last_kv_dump);
    g_global_wchar_logger.clear();
}

TEST(log_pp, macro_wchar_use_global_logger_expect_target_and_kv) {
    g_global_wchar_logger.clear();
    log_pp::set_max_level(log_pp::LevelFilter::Trace);
    log_pp::set_logger(g_global_wchar_logger);

    LOG_PP_TRACE(L"trace {}", 1);
    EXPECT_EQ(L"", g_global_wchar_logger.last_target);
    EXPECT_EQ(L"trace 1", g_global_wchar_logger.last_message);
    EXPECT_EQ(L"", g_global_wchar_logger.last_kv_dump);
    g_global_wchar_logger.clear();

    LOG_PP_TRACE(L"message only");
    EXPECT_EQ(L"", g_global_wchar_logger.last_target);
    EXPECT_EQ(L"message only", g_global_wchar_logger.last_message);
    EXPECT_EQ(L"", g_global_wchar_logger.last_kv_dump);
    g_global_wchar_logger.clear();
}
