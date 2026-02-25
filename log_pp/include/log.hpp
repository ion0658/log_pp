#pragma once

#include <concepts>
#include <format>
#include <functional>
#include <initializer_list>
#include <mutex>
#include <source_location>
#include <string_view>
#include <type_traits>

#include "comptime_filter.hpp"
#include "kv.hpp"
#include "level.hpp"
#include "log_interface.hpp"
#include "metadata.hpp"

#include "log_pp_export.h"

#ifndef __LOG_PP_LOG_HPP__
#define __LOG_PP_LOG_HPP__

namespace log_pp {

/**
 * @brief Sets runtime maximum log level filter.
 *
 * Logs with `level > max_level()` are dropped before a record is built.
 *
 * @param level Maximum runtime level to emit.
 * @return Nothing.
 */
LOG_PP_EXPORT void set_max_level(LevelFilter level) noexcept;
/**
 * @brief Returns the current runtime maximum log level filter.
 *
 * @return Current runtime level filter.
 */
LOG_PP_EXPORT LevelFilter max_level() noexcept;

/**
 * @brief Target/category wrapper used by `log()` overloads.
 *
 * Example:
 * @code
 * LOG_PP_INFO({"http"}, "request done");
 * @endcode
 *
 * @tparam CharT Character type.
 */
template <typename CharT = char>
struct basic_target_t {
    /** @brief Target/category text. */
    std::basic_string_view<CharT> val;
};

template <typename T, typename CharT>
/**
 * @brief Constraint for logger-like types accepted by `log()` overloads.
 *
 * @tparam T Candidate logger type.
 * @tparam CharT Character type.
 */
concept LoggerType = std::derived_from<T, BasicLogger<CharT>> ||
                     std::is_convertible_v<T*, BasicLogger<CharT>*>;

template <typename CharT>
struct default_logger_t : public BasicLogger<CharT> {
    bool enabled(const BasicMetadata<CharT>&) const noexcept override {
        return false;
    }
    void log(const BasicRecord<CharT>&) override {}
    void flush() override {}
};

template <typename CharT>
std::reference_wrapper<BasicLogger<CharT>>& global_logger() noexcept {
    static default_logger_t<CharT> default_logger{};
    static std::reference_wrapper<BasicLogger<CharT>> current = default_logger;
    return current;
}

template <typename CharT>
std::once_flag& logger_flag() noexcept {
    static std::once_flag flag{};
    return flag;
}

template <typename CharT>
bool enabled(const BasicLogger<CharT>& logger,
             Level level,
             std::basic_string_view<CharT> target) {
    return level <= LOG_PP_DETAIL_ACTIVE_COMPILE_LEVEL &&
           level <= log_pp::max_level() &&
           logger.enabled(log_pp::BasicMetadataBuilder<CharT>()
                              .set_level(level)
                              .set_target(target)
                              .build());
}

template <typename CharT = char>
/**
 * @brief Sets process-wide global logger once.
 *
 * The first successful call wins; subsequent calls keep the original logger.
 *
 * Example:
 * @code
 * static MyLogger logger;
 * log_pp::set_logger(logger);
 * @endcode
 *
 * @param logger Logger instance to set as the global logger.
 * @return `true` when `logger` is the active global logger.
 */
bool set_logger(BasicLogger<CharT>& logger) noexcept {
    std::call_once(logger_flag<CharT>(),
                   [&]() { global_logger<CharT>() = logger; });
    return &(global_logger<CharT>().get()) == &logger;
}

template <typename CharT = char>
/**
 * @brief Returns the active global logger reference.
 *
 * Example:
 * @code
 * auto& lg = log_pp::logger();
 * lg.flush();
 * @endcode
 *
 * @return Active global logger reference.
 */
BasicLogger<CharT>& logger() noexcept {
    return global_logger<CharT>();
}

template <typename CharT>
void log_impl(BasicLogger<CharT>& logger,
              Level level,
              std::basic_string_view<CharT> target,
              std::source_location module,
              std::basic_string_view<CharT> fmt,
              FormatArgs<CharT> args,
              std::initializer_list<BasicKV<CharT>> kvs = {}) {
    if (enabled(logger, level, target)) {
        auto record_builder = BasicRecordBuilder<CharT>()
                                  .set_level(level)
                                  .set_target(target)
                                  .set_args(args)
                                  .set_kvs(BasicKVList<CharT>(kvs))
                                  .set_format_string(fmt)
                                  .set_module(module);
        logger.log(record_builder.build());
    }
}

template <typename CharT, typename L, typename... Args>
    requires LoggerType<L, CharT>
/**
 * @brief Logs with explicit logger and target.
 *
 * Prefer using the `LOG_PP_*` macros for automatic source location capture.
 *
 * Example:
 * @code
 * log_pp::log(log_pp::Level::Info,
 *             std::source_location::current(),
 *             my_logger,
 *             {"api"},
 *             "done {}", 42);
 * @endcode
 *
 * @param level Severity level.
 * @param module Source location metadata.
 * @param logger Destination logger.
 * @param target Target/category.
 * @param fmt Format string.
 * @param args Format arguments.
 * @return Nothing.
 */
void log(Level level,
         std::source_location module,
         L& logger,
         basic_target_t<CharT> target,
         std::basic_format_string<CharT, Args...> fmt,
         Args&&... args) {
    if constexpr (std::same_as<CharT, char>) {
        log_impl(logger, level, target.val, module, fmt.get(),
                 std::make_format_args(args...));
    } else if constexpr (std::same_as<CharT, wchar_t>) {
        log_impl(logger, level, target.val, module, fmt.get(),
                 std::make_wformat_args(args...));
    } else {
        auto store = std::make_format_args<FormatContext<CharT>>(args...);
        log_impl(logger, level, target.val, module, fmt.get(),
                 FormatArgs<CharT>(store));
    }
}

template <typename CharT, typename L, typename... Args>
    requires LoggerType<L, CharT>
/**
 * @brief Logs with explicit logger and no target.
 *
 * Example:
 * @code
 * log_pp::log(log_pp::Level::Info,
 *             std::source_location::current(),
 *             my_logger,
 *             "done {}", 42);
 * @endcode
 *
 * @param level Severity level.
 * @param module Source location metadata.
 * @param logger Destination logger.
 * @param fmt Format string.
 * @param args Format arguments.
 * @return Nothing.
 */
void log(Level level,
         std::source_location module,
         L& logger,
         std::basic_format_string<CharT, Args...> fmt,
         Args&&... args) {
    if constexpr (std::same_as<CharT, char>) {
        log_impl(logger, level, {}, module, fmt.get(),
                 std::make_format_args(args...));
    } else if constexpr (std::same_as<CharT, wchar_t>) {
        log_impl(logger, level, {}, module, fmt.get(),
                 std::make_wformat_args(args...));
    } else {
        auto store = std::make_format_args<FormatContext<CharT>>(args...);
        log_impl(logger, level, {}, module, fmt.get(),
                 FormatArgs<CharT>(store));
    }
}

template <typename CharT, typename... Args>
/**
 * @brief Logs to the global logger with explicit target.
 *
 * Example:
 * @code
 * LOG_PP_INFO({"db"}, "connected: {}", true);
 * @endcode
 *
 * @param level Severity level.
 * @param module Source location metadata.
 * @param target Target/category.
 * @param fmt Format string.
 * @param args Format arguments.
 * @return Nothing.
 */
void log(Level level,
         std::source_location module,
         basic_target_t<CharT> target,
         std::basic_format_string<CharT, Args...> fmt,
         Args&&... args) {
    if constexpr (std::same_as<CharT, char>) {
        log_impl(logger<CharT>(), level, target.val, module, fmt.get(),
                 std::make_format_args(args...));
    } else if constexpr (std::same_as<CharT, wchar_t>) {
        log_impl(logger<CharT>(), level, target.val, module, fmt.get(),
                 std::make_wformat_args(args...));
    } else {
        auto store = std::make_format_args<FormatContext<CharT>>(args...);
        log_impl(logger<CharT>(), level, target.val, module, fmt.get(),
                 FormatArgs<CharT>(store));
    }
}

template <typename CharT, typename... Args>
/**
 * @brief Logs to the global logger with no target.
 *
 * Example:
 * @code
 * LOG_PP_INFO("connected: {}", true);
 * @endcode
 *
 * @param level Severity level.
 * @param module Source location metadata.
 * @param fmt Format string.
 * @param args Format arguments.
 * @return Nothing.
 */
void log(Level level,
         std::source_location module,
         std::basic_format_string<CharT, Args...> fmt,
         Args&&... args) {
    if constexpr (std::same_as<CharT, char>) {
        log_impl(logger<CharT>(), level, {}, module, fmt.get(),
                 std::make_format_args(args...));
    } else if constexpr (std::same_as<CharT, wchar_t>) {
        log_impl(logger<CharT>(), level, {}, module, fmt.get(),
                 std::make_wformat_args(args...));
    } else {
        auto store = std::make_format_args<FormatContext<CharT>>(args...);
        log_impl(logger<CharT>(), level, {}, module, fmt.get(),
                 FormatArgs<CharT>(store));
    }
}

template <typename CharT, typename L, typename... Args>
    requires LoggerType<L, CharT>
/**
 * @brief Logs with explicit logger using C-string format.
 *
 * Example:
 * @code
 * log_pp::log(log_pp::Level::Info,
 *             std::source_location::current(),
 *             my_logger,
 *             "done {}", "ok");
 * @endcode
 *
 * @param level Severity level.
 * @param module Source location metadata.
 * @param logger Destination logger.
 * @param fmt C-style format string.
 * @param args Format arguments.
 * @return Nothing.
 */
void log(Level level,
         std::source_location module,
         L& logger,
         const CharT* fmt,
         Args&&... args) {
    if constexpr (std::same_as<CharT, char>) {
        log_impl(logger, level, {}, module, std::basic_string_view<CharT>(fmt),
                 std::make_format_args(args...));
    } else if constexpr (std::same_as<CharT, wchar_t>) {
        log_impl(logger, level, {}, module, std::basic_string_view<CharT>(fmt),
                 std::make_wformat_args(args...));
    } else {
        auto store = std::make_format_args<FormatContext<CharT>>(args...);
        log_impl(logger, level, {}, module, std::basic_string_view<CharT>(fmt),
                 FormatArgs<CharT>(store));
    }
}

template <typename CharT, typename L, typename... Args>
    requires LoggerType<L, CharT>
/**
 * @brief Logs with explicit logger and target using C-string format.
 *
 * Example:
 * @code
 * log_pp::log(log_pp::Level::Info,
 *             std::source_location::current(),
 *             my_logger,
 *             {"api"},
 *             "task {}", 7);
 * @endcode
 *
 * @param level Severity level.
 * @param module Source location metadata.
 * @param logger Destination logger.
 * @param target Target/category.
 * @param fmt C-style format string.
 * @param args Format arguments.
 * @return Nothing.
 */
void log(Level level,
         std::source_location module,
         L& logger,
         basic_target_t<CharT> target,
         const CharT* fmt,
         Args&&... args) {
    if constexpr (std::same_as<CharT, char>) {
        log_impl(logger, level, target.val, module,
                 std::basic_string_view<CharT>(fmt),
                 std::make_format_args(args...));
    } else if constexpr (std::same_as<CharT, wchar_t>) {
        log_impl(logger, level, target.val, module,
                 std::basic_string_view<CharT>(fmt),
                 std::make_wformat_args(args...));
    } else {
        auto store = std::make_format_args<FormatContext<CharT>>(args...);
        log_impl(logger, level, target.val, module,
                 std::basic_string_view<CharT>(fmt), FormatArgs<CharT>(store));
    }
}

template <typename CharT, typename... Args>
/**
 * @brief Logs to the global logger with explicit target and C-string format.
 *
 * Example:
 * @code
 * log_pp::log(log_pp::Level::Info,
 *             std::source_location::current(),
 *             {"db"},
 *             "connected {}", true);
 * @endcode
 *
 * @param level Severity level.
 * @param module Source location metadata.
 * @param target Target/category.
 * @param fmt C-style format string.
 * @param args Format arguments.
 * @return Nothing.
 */
void log(Level level,
         std::source_location module,
         basic_target_t<CharT> target,
         const CharT* fmt,
         Args&&... args) {
    if constexpr (std::same_as<CharT, char>) {
        log_impl(logger<CharT>(), level, target.val, module,
                 std::basic_string_view<CharT>(fmt),
                 std::make_format_args(args...));
    } else if constexpr (std::same_as<CharT, wchar_t>) {
        log_impl(logger<CharT>(), level, target.val, module,
                 std::basic_string_view<CharT>(fmt),
                 std::make_wformat_args(args...));
    } else {
        auto store = std::make_format_args<FormatContext<CharT>>(args...);
        log_impl(logger<CharT>(), level, target.val, module,
                 std::basic_string_view<CharT>(fmt), FormatArgs<CharT>(store));
    }
}

template <typename CharT, typename... Args>
/**
 * @brief Logs to the global logger using C-string format.
 *
 * Example:
 * @code
 * log_pp::log(log_pp::Level::Info,
 *             std::source_location::current(),
 *             "connected {}", true);
 * @endcode
 *
 * @param level Severity level.
 * @param module Source location metadata.
 * @param fmt C-style format string.
 * @param args Format arguments.
 * @return Nothing.
 */
void log(Level level,
         std::source_location module,
         const CharT* fmt,
         Args&&... args) {
    if constexpr (std::same_as<CharT, char>) {
        log_impl(logger<CharT>(), level, {}, module,
                 std::basic_string_view<CharT>(fmt),
                 std::make_format_args(args...));
    } else if constexpr (std::same_as<CharT, wchar_t>) {
        log_impl(logger<CharT>(), level, {}, module,
                 std::basic_string_view<CharT>(fmt),
                 std::make_wformat_args(args...));
    } else {
        auto store = std::make_format_args<FormatContext<CharT>>(args...);
        log_impl(logger<CharT>(), level, {}, module,
                 std::basic_string_view<CharT>(fmt), FormatArgs<CharT>(store));
    }
}

template <typename CharT, typename L, typename... Args>
    requires LoggerType<L, CharT>
/**
 * @brief Logs with explicit logger and key-value fields.
 *
 * Example:
 * @code
 * log_pp::log(log_pp::Level::Info,
 *             std::source_location::current(),
 *             my_logger,
 *             {{"request_id", 42}},
 *             "done {}", true);
 * @endcode
 *
 * @param level Severity level.
 * @param module Source location metadata.
 * @param logger Destination logger.
 * @param kvs Structured key-value fields.
 * @param fmt Format string.
 * @param args Format arguments.
 * @return Nothing.
 */
void log(Level level,
         std::source_location module,
         L& logger,
         std::initializer_list<BasicKV<CharT>> kvs,
         std::basic_format_string<CharT, Args...> fmt,
         Args&&... args) {
    if constexpr (std::same_as<CharT, char>) {
        log_impl(logger, level, {}, module, fmt.get(),
                 std::make_format_args(args...), kvs);
    } else if constexpr (std::same_as<CharT, wchar_t>) {
        log_impl(logger, level, {}, module, fmt.get(),
                 std::make_wformat_args(args...), kvs);
    } else {
        auto store = std::make_format_args<FormatContext<CharT>>(args...);
        log_impl(logger, level, {}, module, fmt.get(), FormatArgs<CharT>(store),
                 kvs);
    }
}

template <typename CharT, typename L, typename... Args>
    requires LoggerType<L, CharT>
/**
 * @brief Logs with explicit logger, target, and key-value fields.
 *
 * Example:
 * @code
 * log_pp::log(log_pp::Level::Info,
 *             std::source_location::current(),
 *             my_logger,
 *             {"api"},
 *             {{"status", "ok"}},
 *             "done");
 * @endcode
 *
 * @param level Severity level.
 * @param module Source location metadata.
 * @param logger Destination logger.
 * @param target Target/category.
 * @param kvs Structured key-value fields.
 * @param fmt Format string.
 * @param args Format arguments.
 * @return Nothing.
 */
void log(Level level,
         std::source_location module,
         L& logger,
         basic_target_t<CharT> target,
         std::initializer_list<BasicKV<CharT>> kvs,
         std::basic_format_string<CharT, Args...> fmt,
         Args&&... args) {
    if constexpr (std::same_as<CharT, char>) {
        log_impl(logger, level, target.val, module, fmt.get(),
                 std::make_format_args(args...), kvs);
    } else if constexpr (std::same_as<CharT, wchar_t>) {
        log_impl(logger, level, target.val, module, fmt.get(),
                 std::make_wformat_args(args...), kvs);
    } else {
        auto store = std::make_format_args<FormatContext<CharT>>(args...);
        log_impl(logger, level, target.val, module, fmt.get(),
                 FormatArgs<CharT>(store), kvs);
    }
}

template <typename CharT, typename... Args>
/**
 * @brief Logs to the global logger with optional key-value fields.
 *
 * Example:
 * @code
 * LOG_PP_INFO({{"request_id", 42}, {"user", "alice"}},
 *             "processed {} items", 10);
 * @endcode
 *
 * @param level Severity level.
 * @param module Source location metadata.
 * @param kvs Structured key-value fields.
 * @param fmt Format string.
 * @param args Format arguments.
 * @return Nothing.
 */
void log(Level level,
         std::source_location module,
         std::initializer_list<BasicKV<CharT>> kvs,
         std::basic_format_string<CharT, Args...> fmt,
         Args&&... args) {
    if constexpr (std::same_as<CharT, char>) {
        log_impl(logger<CharT>(), level, {}, module, fmt.get(),
                 std::make_format_args(args...), kvs);
    } else if constexpr (std::same_as<CharT, wchar_t>) {
        log_impl(logger<CharT>(), level, {}, module, fmt.get(),
                 std::make_wformat_args(args...), kvs);
    } else {
        auto store = std::make_format_args<FormatContext<CharT>>(args...);
        log_impl(logger<CharT>(), level, {}, module, fmt.get(),
                 FormatArgs<CharT>(store), kvs);
    }
}

template <typename CharT, typename... Args>
/**
 * @brief Logs to the global logger with target and key-value fields.
 *
 * Example:
 * @code
 * LOG_PP_INFO({"api"}, {{"status", "ok"}}, "done");
 * @endcode
 *
 * @param level Severity level.
 * @param module Source location metadata.
 * @param target Target/category.
 * @param kvs Structured key-value fields.
 * @param fmt Format string.
 * @param args Format arguments.
 * @return Nothing.
 */
void log(Level level,
         std::source_location module,
         basic_target_t<CharT> target,
         std::initializer_list<BasicKV<CharT>> kvs,
         std::basic_format_string<CharT, Args...> fmt,
         Args&&... args) {
    if constexpr (std::same_as<CharT, char>) {
        log_impl(logger<CharT>(), level, target.val, module, fmt.get(),
                 std::make_format_args(args...), kvs);
    } else if constexpr (std::same_as<CharT, wchar_t>) {
        log_impl(logger<CharT>(), level, target.val, module, fmt.get(),
                 std::make_wformat_args(args...), kvs);
    } else {
        auto store = std::make_format_args<FormatContext<CharT>>(args...);
        log_impl(logger<CharT>(), level, target.val, module, fmt.get(),
                 FormatArgs<CharT>(store), kvs);
    }
}

template <typename CharT, typename L, typename... Args>
    requires LoggerType<L, CharT>
/**
 * @brief Logs with explicit logger and key-value fields using C-string format.
 *
 * Example:
 * @code
 * log_pp::log(log_pp::Level::Info,
 *             std::source_location::current(),
 *             my_logger,
 *             {{"request_id", 42}},
 *             "done {}", true);
 * @endcode
 *
 * @param level Severity level.
 * @param module Source location metadata.
 * @param logger Destination logger.
 * @param kvs Structured key-value fields.
 * @param fmt C-style format string.
 * @param args Format arguments.
 * @return Nothing.
 */
void log(Level level,
         std::source_location module,
         L& logger,
         std::initializer_list<BasicKV<CharT>> kvs,
         const CharT* fmt,
         Args&&... args) {
    if constexpr (std::same_as<CharT, char>) {
        log_impl(logger, level, {}, module, std::basic_string_view<CharT>(fmt),
                 std::make_format_args(args...), kvs);
    } else if constexpr (std::same_as<CharT, wchar_t>) {
        log_impl(logger, level, {}, module, std::basic_string_view<CharT>(fmt),
                 std::make_wformat_args(args...), kvs);
    } else {
        auto store = std::make_format_args<FormatContext<CharT>>(args...);
        log_impl(logger, level, {}, module, std::basic_string_view<CharT>(fmt),
                 FormatArgs<CharT>(store), kvs);
    }
}

template <typename CharT, typename L, typename... Args>
    requires LoggerType<L, CharT>
/**
 * @brief Logs with explicit logger, target, key-values, and C-string format.
 *
 * Example:
 * @code
 * log_pp::log(log_pp::Level::Info,
 *             std::source_location::current(),
 *             my_logger,
 *             {"api"},
 *             {{"status", "ok"}},
 *             "done");
 * @endcode
 *
 * @param level Severity level.
 * @param module Source location metadata.
 * @param logger Destination logger.
 * @param target Target/category.
 * @param kvs Structured key-value fields.
 * @param fmt C-style format string.
 * @param args Format arguments.
 * @return Nothing.
 */
void log(Level level,
         std::source_location module,
         L& logger,
         basic_target_t<CharT> target,
         std::initializer_list<BasicKV<CharT>> kvs,
         const CharT* fmt,
         Args&&... args) {
    if constexpr (std::same_as<CharT, char>) {
        log_impl(logger, level, target.val, module,
                 std::basic_string_view<CharT>(fmt),
                 std::make_format_args(args...), kvs);
    } else if constexpr (std::same_as<CharT, wchar_t>) {
        log_impl(logger, level, target.val, module,
                 std::basic_string_view<CharT>(fmt),
                 std::make_wformat_args(args...), kvs);
    } else {
        auto store = std::make_format_args<FormatContext<CharT>>(args...);
        log_impl(logger, level, target.val, module,
                 std::basic_string_view<CharT>(fmt), FormatArgs<CharT>(store),
                 kvs);
    }
}

template <typename CharT, typename... Args>
/**
 * @brief Logs to the global logger with key-values using C-string format.
 *
 * Example:
 * @code
 * log_pp::log(log_pp::Level::Info,
 *             std::source_location::current(),
 *             {{"request_id", 42}},
 *             "done");
 * @endcode
 *
 * @param level Severity level.
 * @param module Source location metadata.
 * @param kvs Structured key-value fields.
 * @param fmt C-style format string.
 * @param args Format arguments.
 * @return Nothing.
 */
void log(Level level,
         std::source_location module,
         std::initializer_list<BasicKV<CharT>> kvs,
         const CharT* fmt,
         Args&&... args) {
    if constexpr (std::same_as<CharT, char>) {
        log_impl(logger<CharT>(), level, {}, module,
                 std::basic_string_view<CharT>(fmt),
                 std::make_format_args(args...), kvs);
    } else if constexpr (std::same_as<CharT, wchar_t>) {
        log_impl(logger<CharT>(), level, {}, module,
                 std::basic_string_view<CharT>(fmt),
                 std::make_wformat_args(args...), kvs);
    } else {
        auto store = std::make_format_args<FormatContext<CharT>>(args...);
        log_impl(logger<CharT>(), level, {}, module,
                 std::basic_string_view<CharT>(fmt), FormatArgs<CharT>(store),
                 kvs);
    }
}

template <typename CharT, typename... Args>
/**
 * @brief Logs to the global logger with target/key-values using C-string
 * format.
 *
 * Example:
 * @code
 * log_pp::log(log_pp::Level::Info,
 *             std::source_location::current(),
 *             {"api"},
 *             {{"status", "ok"}},
 *             "done");
 * @endcode
 *
 * @param level Severity level.
 * @param module Source location metadata.
 * @param target Target/category.
 * @param kvs Structured key-value fields.
 * @param fmt C-style format string.
 * @param args Format arguments.
 * @return Nothing.
 */
void log(Level level,
         std::source_location module,
         basic_target_t<CharT> target,
         std::initializer_list<BasicKV<CharT>> kvs,
         const CharT* fmt,
         Args&&... args) {
    if constexpr (std::same_as<CharT, char>) {
        log_impl(logger<CharT>(), level, target.val, module,
                 std::basic_string_view<CharT>(fmt),
                 std::make_format_args(args...), kvs);
    } else if constexpr (std::same_as<CharT, wchar_t>) {
        log_impl(logger<CharT>(), level, target.val, module,
                 std::basic_string_view<CharT>(fmt),
                 std::make_wformat_args(args...), kvs);
    } else {
        auto store = std::make_format_args<FormatContext<CharT>>(args...);
        log_impl(logger<CharT>(), level, target.val, module,
                 std::basic_string_view<CharT>(fmt), FormatArgs<CharT>(store),
                 kvs);
    }
}

}  // namespace log_pp

/**
 * @brief Base logging macro used by severity macros.
 *
 * Example:
 * @code
 * LOG_PP(log_pp::Level::Info, "hello {}", "world");
 * @endcode
 *
 * @param level Severity level.
 * @return Nothing.
 */
#define LOG_PP(level, ...) \
    log_pp::log(level, std::source_location::current(), __VA_ARGS__);

/**
 * @brief Logs at TRACE severity.
 *
 * Example: `LOG_PP_TRACE("x={}", x);`
 */
#define LOG_PP_TRACE(...) LOG_PP(log_pp::Level::Trace, __VA_ARGS__)

/**
 * @brief Logs at DEBUG severity.
 *
 * Example: `LOG_PP_DEBUG("state={}", state);`
 */
#define LOG_PP_DEBUG(...) LOG_PP(log_pp::Level::Debug, __VA_ARGS__)

/**
 * @brief Logs at INFO severity.
 *
 * Example: `LOG_PP_INFO("ready");`
 */
#define LOG_PP_INFO(...) LOG_PP(log_pp::Level::Info, __VA_ARGS__)

/**
 * @brief Logs at WARNING severity.
 *
 * Example: `LOG_PP_WARN("retrying {}", id);`
 */
#define LOG_PP_WARN(...) LOG_PP(log_pp::Level::Warning, __VA_ARGS__)

/**
 * @brief Logs at ERROR severity.
 *
 * Example: `LOG_PP_ERROR("failed: {}", reason);`
 */
#define LOG_PP_ERROR(...) LOG_PP(log_pp::Level::Error, __VA_ARGS__)

#endif  // !__LOG_PP_LOG_HPP__
