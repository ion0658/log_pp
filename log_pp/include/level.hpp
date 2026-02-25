#pragma once

#include <algorithm>
#include <compare>
#include <format>

#ifndef __LOG_PP_LEVEL_HPP__
#define __LOG_PP_LEVEL_HPP__

namespace log_pp {

/**
 * @brief Runtime log severity.
 *
 * The order is `Error < Warning < Info < Debug < Trace`.
 *
 * Example: `if (level >= log_pp::Level::Info) { ... }`
 */
enum class Level {
    Error = 1,
    Warning,
    Info,
    Debug,
    Trace,
};

constexpr std::strong_ordering operator<=>(const Level lhs,
                                           const Level rhs) noexcept {
    return static_cast<int>(lhs) <=> static_cast<int>(rhs);
}
constexpr std::strong_ordering operator<=>(const Level lhs,
                                           const int rhs) noexcept {
    return static_cast<int>(lhs) <=> rhs;
}
constexpr std::strong_ordering operator<=>(const int lhs,
                                           const Level rhs) noexcept {
    return lhs <=> static_cast<int>(rhs);
}

constexpr bool operator==(Level lhs, Level rhs) noexcept {
    return static_cast<int>(lhs) == static_cast<int>(rhs);
}
constexpr bool operator==(Level lhs, int rhs) noexcept {
    return static_cast<int>(lhs) == rhs;
}
constexpr bool operator==(int lhs, Level rhs) noexcept {
    return rhs == lhs;
}

constexpr bool operator!=(Level lhs, Level rhs) noexcept {
    return !(lhs == rhs);
}
constexpr bool operator!=(Level lhs, int rhs) noexcept {
    return !(lhs == rhs);
}
constexpr bool operator!=(int lhs, Level rhs) noexcept {
    return !(rhs == lhs);
}

constexpr Level operator++(const Level& lhs, const int) noexcept {
    return static_cast<Level>(std::clamp(static_cast<int>(lhs) + 1,
                                         static_cast<int>(Level::Error),
                                         static_cast<int>(Level::Trace)));
}
constexpr Level operator++(Level& lhs, const int) noexcept {
    lhs = static_cast<Level>(std::clamp(static_cast<int>(lhs) + 1,
                                        static_cast<int>(Level::Error),
                                        static_cast<int>(Level::Trace)));
    return lhs;
}
constexpr Level operator+=(Level& lhs, const int offset) noexcept {
    lhs = static_cast<Level>(std::clamp(static_cast<int>(lhs) + offset,
                                        static_cast<int>(Level::Error),
                                        static_cast<int>(Level::Trace)));
    return lhs;
}

constexpr Level operator--(const Level& lhs, const int) noexcept {
    return static_cast<Level>(std::clamp(static_cast<int>(lhs) - 1,
                                         static_cast<int>(Level::Error),
                                         static_cast<int>(Level::Trace)));
}
constexpr Level operator--(Level& lhs, const int) noexcept {
    lhs = static_cast<Level>(std::clamp(static_cast<int>(lhs) - 1,
                                        static_cast<int>(Level::Error),
                                        static_cast<int>(Level::Trace)));
    return lhs;
}
constexpr Level operator-=(Level& lhs, const int offset) noexcept {
    lhs = static_cast<Level>(std::clamp(static_cast<int>(lhs) - offset,
                                        static_cast<int>(Level::Error),
                                        static_cast<int>(Level::Trace)));
    return lhs;
}

/**
 * @brief Converts a level to an uppercase string representation.
 *
 * @param level Log level.
 * @return Uppercase level string such as `"INFO"`.
 */
constexpr const char* to_str(const Level& level) noexcept {
    switch (level) {
        case Level::Error:
            return "ERROR";
        case Level::Warning:
            return "WARNING";
        case Level::Info:
            return "INFO";
        case Level::Debug:
            return "DEBUG";
        case Level::Trace:
            return "TRACE";
        default:
            return "UNKNOWN";
    }
}

/**
 * @brief Runtime level filter.
 *
 * `Off` disables all logs, and higher values allow more verbose logs.
 *
 * Example: `log_pp::set_max_level(log_pp::LevelFilter::Warn);`
 */
enum class LevelFilter {
    Off,
    Error,
    Warn,
    Info,
    Debug,
    Trace,
};

constexpr std::strong_ordering operator<=>(const LevelFilter lhs,
                                           const LevelFilter rhs) noexcept {
    return static_cast<int>(lhs) <=> static_cast<int>(rhs);
}
constexpr std::strong_ordering operator<=>(const LevelFilter lhs,
                                           const Level rhs) noexcept {
    return static_cast<int>(lhs) <=> static_cast<int>(rhs);
}
constexpr std::strong_ordering operator<=>(const Level lhs,
                                           const LevelFilter rhs) noexcept {
    return static_cast<int>(lhs) <=> static_cast<int>(rhs);
}

constexpr bool operator==(LevelFilter lhs, LevelFilter rhs) noexcept {
    return static_cast<int>(lhs) == static_cast<int>(rhs);
}
constexpr bool operator==(LevelFilter lhs, Level rhs) noexcept {
    return static_cast<int>(lhs) == static_cast<int>(rhs);
}
constexpr bool operator==(Level lhs, LevelFilter rhs) noexcept {
    return rhs == lhs;
}
constexpr bool operator!=(LevelFilter lhs, LevelFilter rhs) noexcept {
    return !(lhs == rhs);
}
constexpr bool operator!=(LevelFilter lhs, Level rhs) noexcept {
    return !(lhs == rhs);
}
constexpr bool operator!=(Level lhs, LevelFilter rhs) noexcept {
    return !(rhs == lhs);
}

constexpr LevelFilter operator++(const LevelFilter& lhs, const int) noexcept {
    return static_cast<LevelFilter>(std::clamp(
        static_cast<int>(lhs) + 1, static_cast<int>(LevelFilter::Off),
        static_cast<int>(LevelFilter::Trace)));
}
constexpr LevelFilter operator++(LevelFilter& lhs, const int) noexcept {
    lhs = static_cast<LevelFilter>(std::clamp(
        static_cast<int>(lhs) + 1, static_cast<int>(LevelFilter::Off),
        static_cast<int>(LevelFilter::Trace)));
    return lhs;
}
constexpr LevelFilter operator+=(LevelFilter& lhs, const int offset) noexcept {
    lhs = static_cast<LevelFilter>(std::clamp(
        static_cast<int>(lhs) + offset, static_cast<int>(LevelFilter::Off),
        static_cast<int>(LevelFilter::Trace)));
    return lhs;
}
constexpr LevelFilter operator--(const LevelFilter& lhs, const int) noexcept {
    return static_cast<LevelFilter>(std::clamp(
        static_cast<int>(lhs) - 1, static_cast<int>(LevelFilter::Off),
        static_cast<int>(LevelFilter::Trace)));
}
constexpr LevelFilter operator--(LevelFilter& lhs, const int) noexcept {
    lhs = static_cast<LevelFilter>(std::clamp(
        static_cast<int>(lhs) - 1, static_cast<int>(LevelFilter::Off),
        static_cast<int>(LevelFilter::Trace)));
    return lhs;
}
constexpr LevelFilter operator-=(LevelFilter& lhs, const int offset) noexcept {
    lhs = static_cast<LevelFilter>(std::clamp(
        static_cast<int>(lhs) - offset, static_cast<int>(LevelFilter::Off),
        static_cast<int>(LevelFilter::Trace)));
    return lhs;
}

}  // namespace log_pp

template <typename CharT>
struct std::formatter<log_pp::Level, CharT> {
    constexpr auto parse(std::basic_format_parse_context<CharT>& ctx) {
        return ctx.begin();
    }

    template <typename FormatContext>
    constexpr auto format(const log_pp::Level& level,
                          FormatContext& ctx) const {
        return std::format_to(ctx.out(), "{}", log_pp::to_str(level));
    }
};

template <typename CharT>
struct std::formatter<log_pp::LevelFilter, CharT> {
    constexpr auto parse(std::basic_format_parse_context<CharT>& ctx) {
        return ctx.begin();
    }

    template <typename FormatContext>
    constexpr auto format(const log_pp::LevelFilter& level_filter,
                          FormatContext& ctx) const {
        return std::format_to(ctx.out(), "{}",
                              static_cast<log_pp::Level>(level_filter));
    }
};

#endif  // !__LOG_PP_LEVEL_HPP__
