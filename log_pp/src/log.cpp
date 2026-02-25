#include <atomic>

#include "level.hpp"
#include "log.hpp"

namespace {
std::atomic<log_pp::LevelFilter> MAX_LOG_LEVEL_FILTER =
    log_pp::LevelFilter::Trace;

}  // namespace

namespace log_pp {
void set_max_level(LevelFilter level) noexcept {
    MAX_LOG_LEVEL_FILTER.store(level);
}

LevelFilter max_level() noexcept {
    return MAX_LOG_LEVEL_FILTER.load();
}

}  // namespace log_pp
