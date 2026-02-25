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

    LOG_PP_TRACE("Hello, {}!", "world");
    LOG_PP_DEBUG({"example"}, "processed {} items", 12);
    LOG_PP_INFO({{"request_id", 42}, {"user", "alice"}, {"ok", true}},
                "request {} completed in {} ms", "A-01", 12.3);
    LOG_PP_WARN({"example"},
                {{"hex_id", 255, "0x{:04X}"},
                 {"latency_ms", 12.345, "{:.2f}"},
                 {"retries", 2}},
                "custom kv format for {}", "request");
    LOG_PP_ERROR({"example"},
                 {{"attempt", 2}, {"backoff_ms", 250}, {"state", "retry"}},
                 "retrying {}", "upload");

    return 0;
}
