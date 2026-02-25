#include <format>
#include <iostream>
#include <numeric>
#include <string>

#include "log.hpp"

struct SimpleLogger : public log_pp::ILogger {
    virtual bool enabled(const log_pp::Metadata& meta) const noexcept {
        if (meta.get_target() == "enabled_target") {
            return true;
        }
        if (meta.get_target() == "disabled_target") {
            return false;
        }
        if (meta.get_target() == "info_target") {
            return meta.get_level() <= log_pp::Level::Info;
        }
        return true;
    }

    virtual void log(const log_pp::Record& record) noexcept {
        std::string kv_text = std::accumulate(
            record.get_kvs().begin(), record.get_kvs().end(), std::string{},
            [](const auto& acc, const auto& kv) {
                return acc +
                       std::format("{}: {}, ", kv.get_key(), kv.get_value());
            });
        auto message =
            std::vformat(record.get_format_string(), record.get_args());
        std::cout << std::format("[{}] [{}] {}{}", record.get_level(),
                                 record.get_target(), kv_text, message)
                  << std::endl;
    }

    virtual void flush() noexcept { std::cout.flush(); }
};

int main() {
    static SimpleLogger logger;
    log_pp::set_logger(logger);
    log_pp::set_max_level(log_pp::LevelFilter::Trace);

    LOG_PP_TRACE({"enabled_target"},
                 "This message will be logged because the target is enabled "
                 "and the level is Trace");
    LOG_PP_DEBUG({"enabled_target"},
                 "This message will be logged because the target is enabled "
                 "and the level is Debug");
    LOG_PP_INFO({"enabled_target"},
                "This message will be logged because the target is enabled "
                "and the level is Info");
    LOG_PP_WARN({"enabled_target"},
                "This message will be logged because the target is enabled "
                "and the level is Warn");
    LOG_PP_ERROR({"enabled_target"},
                 "This message will be logged because the target is enabled "
                 "and the level is Error");

    LOG_PP_TRACE(
        {"disabled_target"},
        "This message will NOT be logged because the target is disabled");
    LOG_PP_DEBUG(
        {"disabled_target"},
        "This message will NOT be logged because the target is disabled");
    LOG_PP_INFO(
        {"disabled_target"},
        "This message will NOT be logged because the target is disabled");
    LOG_PP_WARN(
        {"disabled_target"},
        "This message will NOT be logged because the target is disabled");
    LOG_PP_ERROR(
        {"disabled_target"},
        "This message will NOT be logged because the target is disabled");

    LOG_PP_TRACE(
        {"info_target"},
        "This message will NOT be logged because the level is Trace and the "
        "target only allows levels up to Info");
    LOG_PP_DEBUG(
        {"info_target"},
        "This message will NOT be logged because the level is Debug and the "
        "target only allows levels up to Info");
    LOG_PP_INFO(
        {"info_target"},
        "This message will be logged because the level is Info and the target "
        "allows levels up to Info");
    LOG_PP_WARN(
        {"info_target"},
        "This message will be logged because the level is Warn and the target "
        "allows levels up to Info");
    LOG_PP_ERROR(
        {"info_target"},
        "This message will be logged because the level is Error and the target "
        "allows levels up to Info");

    LOG_PP_TRACE({"other_target"},
                 "This message will be logged because the target is not "
                 "specifically disabled");
    LOG_PP_TRACE(
        "This message will be logged because the target is not specified");

    return 0;
}
