#include <format>
#include <iostream>
#include <numeric>
#include <string>

#include "log.hpp"

struct SimpleLogger : public log_pp::ILogger {
    virtual bool enabled(const log_pp::Metadata&) const noexcept {
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

    LOG_PP_TRACE(
        "This message will not be logged because the compile-time filter will "
        "return always false");
    LOG_PP_DEBUG(
        "This message will be logged in debug only because the compile-time "
        "filter will return true only in debug builds");

    LOG_PP_INFO(
        "This message will be logged because the compile-time filter will "
        "return true");
    LOG_PP_WARN(
        "This message will be logged because the compile-time filter will "
        "return true");
    LOG_PP_ERROR(
        "This message will be logged because the compile-time filter will "
        "return true");

    return 0;
}
