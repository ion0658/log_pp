#include <cassert>
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
        std::cout << std::format("SimpleLogger: [{}] [{}] {}{}",
                                 record.get_level(), record.get_target(),
                                 kv_text, message)
                  << std::endl;
    }

    virtual void flush() noexcept { std::cout.flush(); }
};

struct LocalLogger : public log_pp::ILogger {
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
        std::cout << std::format("LocalLogger: [{}] [{}] {}{}",
                                 record.get_level(), record.get_target(),
                                 kv_text, message)
                  << std::endl;
    }

    virtual void flush() noexcept { std::cout.flush(); }
};

int main() {
    static SimpleLogger logger;
    // set_logger returns true if the logger is set successfully, and it should
    // succeed
    assert(log_pp::set_logger(logger));
    log_pp::set_max_level(log_pp::LevelFilter::Trace);

    LOG_PP_INFO("This message will be logged by global logger");

    {
        LocalLogger local_logger;
        LOG_PP_INFO("This message will be logged by global logger");
        LOG_PP_INFO(local_logger, "This message will be logged by LocalLogger");
    }

    static LocalLogger local_logger;
    // set_logger returns false if the logger is already set, so it should fail
    // set_logger can only be called once, and it doew not change global logger
    // after the first call and it will return false for subsequent calls
    assert(!log_pp::set_logger(local_logger));
    LOG_PP_INFO("This message will still be logged by global logger");
    LOG_PP_INFO(local_logger,
                "Global logger can't change but can use local logger");

    return 0;
}
