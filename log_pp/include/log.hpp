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
#include "record.hpp"

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
    return level <= log_pp::get_comptime_level() &&
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

template <typename CharT, LoggerType<CharT> L>
void log_impl(L& logger,
              Level level,
              std::basic_string_view<CharT> target,
              std::source_location module,
              std::initializer_list<BasicKV<CharT>> kvs,
              std::basic_string_view<CharT> fmt,
              FormatArgs<CharT> args) {
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

template <typename CharT, LoggerType<CharT> L, typename... Args>
/**
 * @brief Logging function with explicit logger parameter.
 *
 * Prefer using the `LOG_PP_*` macros for automatic source location capture.
 *
 * Example:
 * @code
 * LOG_INFO(my_logger,
 *          {"api"},
 *          {{"key", "value"}, {"attempt", 3}},
 *          "done {}", 42);
 * @endcode
 *
 * @param level Severity level.
 * @param module Source location metadata.
 * @param logger Destination logger.
 * @param target Target/category.
 * @param kvs Key-value pairs.
 * @param fmt Format string.
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
        log_impl(logger, level, target.val, module, kvs, {fmt},
                 std::make_format_args(args...));
    } else if constexpr (std::same_as<CharT, wchar_t>) {
        log_impl(logger, level, target.val, module, kvs, {fmt},
                 std::make_wformat_args(args...));
    } else {
        auto store = std::make_format_args<CharT>(args...);
        log_impl(logger, level, target.val, module, kvs, {fmt},
                 FormatArgs<CharT>(store));
    }
}

template <typename CharT, LoggerType<CharT> L, typename... Args>
/**
 * @brief Overload of `log()` without key-value pairs.
 *
 * Prefer using the `LOG_PP_*` macros for automatic source location capture.
 *
 * Example:
 * @code
 * LOG_INFO(my_logger,
 *          {"api"},
 *          "done {}", 42);
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
         const CharT* fmt,
         Args&&... args) {
    log(level, module, logger, target, {}, fmt, std::forward<Args>(args)...);
}

template <typename CharT, LoggerType<CharT> L, typename... Args>
/**
 * @brief Overload of `log()` without target.
 *
 * Prefer using the `LOG_PP_*` macros for automatic source location capture.
 *
 * Example:
 * @code
 * LOG_INFO(my_logger,
 *          {{"key", "value"}, {"attempt", 3}},
 *          "done {}", 42);
 * @endcode
 *
 * @param level Severity level.
 * @param module Source location metadata.
 * @param logger Destination logger.
 * @param kvs Key-value pairs.
 * @param fmt Format string.
 * @param args Format arguments.
 * @return Nothing.
 */
void log(Level level,
         std::source_location module,
         L& logger,
         std::initializer_list<BasicKV<CharT>> kvs,
         const CharT* fmt,
         Args&&... args) {
    log(level, module, logger, basic_target_t<CharT>{}, kvs, fmt,
        std::forward<Args>(args)...);
}

template <typename CharT, LoggerType<CharT> L, typename... Args>
/**
 * @brief Overload of `log()` without target and key-value pairs.
 *
 * Prefer using the `LOG_PP_*` macros for automatic source location capture.
 *
 * Example:
 * @code
 * LOG_INFO(my_logger, "done {}", 42);
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
         const CharT* fmt,
         Args&&... args) {
    log(level, module, logger, basic_target_t<CharT>{}, {}, fmt,
        std::forward<Args>(args)...);
}

template <typename CharT, LoggerType<CharT> L>
/**
 * @brief Overload of `log()` without message.
 *
 * Prefer using the `LOG_PP_*` macros for automatic source location capture.
 *
 * Example:
 * @code
 * LOG_INFO(my_logger,
 *          log_pp::basic_target_t<char>{"api"},
 *          {{"key", "value"}, {"attempt", 3}});
 * @endcode
 *
 * @param level Severity level.
 * @param module Source location metadata.
 * @param logger Destination logger.
 * @param target Target/category.
 * @param kvs Key-value pairs.
 * @return Nothing.
 */
void log(Level level,
         std::source_location module,
         L& logger,
         basic_target_t<CharT> target,
         std::initializer_list<BasicKV<CharT>> kvs) {
    if constexpr (std::same_as<CharT, char>) {
        log_impl(logger, level, target.val, module, kvs, {},
                 std::make_format_args());
    } else if constexpr (std::same_as<CharT, wchar_t>) {
        log_impl(logger, level, target.val, module, kvs, {},
                 std::make_wformat_args());
    } else {
        auto store = std::make_format_args<CharT>();
        log_impl(logger, level, target.val, module, kvs, {},
                 FormatArgs<CharT>(store));
    }
}

template <typename CharT, LoggerType<CharT> L>
/**
 * @brief Overload of `log()` without message.
 *
 * Prefer using the `LOG_PP_*` macros for automatic source location capture.
 *
 * Example:
 * @code
 * LOG_INFO(my_logger,
 *          {"api"},
 *          {{"key", "value"}, {"attempt", 3}});
 * @endcode
 *
 * @param level Severity level.
 * @param module Source location metadata.
 * @param logger Destination logger.
 * @param target Target/category.
 * @param kvs Key-value pairs.
 * @return Nothing.
 */
void log(Level level,
         std::source_location module,
         L& logger,
         std::initializer_list<const CharT*> target,
         std::initializer_list<BasicKV<CharT>> kvs) {
    if (target.size() > 0) {
        log(level, module, logger, basic_target_t<CharT>{*target.begin()}, kvs);
    } else {
        log(level, module, logger, basic_target_t<CharT>{}, kvs);
    }
}

template <typename CharT, LoggerType<CharT> L>
/**
 * @brief Overload of `log()` without message and target.
 *
 * Prefer using the `LOG_PP_*` macros for automatic source location capture.
 *
 * Example:
 * @code
 * LOG_INFO(my_logger,
 *          std::initializer_list<log_pp::BasicKV<char>>{
 *                                                          {"key", "value"},
 *                                                          {"attempt", 3}
 *                                                      }
 *          );
 * @endcode
 *
 * @param level Severity level.
 * @param module Source location metadata.
 * @param logger Destination logger.
 * @param kvs Key-value pairs.
 * @return Nothing.
 */
void log(Level level,
         std::source_location module,
         L& logger,
         std::initializer_list<BasicKV<CharT>> kvs) {
    log(level, module, logger, basic_target_t<CharT>{}, kvs);
}

// =================================================================

template <typename CharT, typename... Args>
/**
 * @brief Overload of `log()` using global logger.
 *
 * Prefer using the `LOG_PP_*` macros for automatic source location capture.
 *
 * Example:
 * @code
 * LOG_INFO({"api"},
 *          {{"key", "value"}, {"attempt", 3}},
 *          "done {}", 42);
 * @endcode
 *
 * @param level Severity level.
 * @param module Source location metadata.
 * @param target Target/category.
 * @param kvs Key-value pairs.
 * @param fmt Format string.
 * @param args Format arguments.
 * @return Nothing.
 */
void log(Level level,
         std::source_location module,
         basic_target_t<CharT> target,
         std::initializer_list<BasicKV<CharT>> kvs,
         const CharT* fmt,
         Args&&... args) {
    log(level, module, logger<CharT>(), target, kvs, fmt,
        std::forward<Args>(args)...);
}

template <typename CharT, typename... Args>
/**
 * @brief Overload of `log()` using global logger without key-value pairs.
 *
 * Prefer using the `LOG_PP_*` macros for automatic source location capture.
 *
 * Example:
 * @code
 * LOG_INFO({"api"},
 *          "done {}", 42);
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
         const CharT* fmt,
         Args&&... args) {
    log(level, module, logger<CharT>(), target, {}, fmt,
        std::forward<Args>(args)...);
}

template <typename CharT, typename... Args>
/**
 * @brief Overload of `log()` using global logger without target.
 *
 * Prefer using the `LOG_PP_*` macros for automatic source location capture.
 *
 * Example:
 * @code
 * LOG_INFO({{"key", "value"}, {"attempt", 3}},
 *          "done {}", 42);
 * @endcode
 *
 * @param level Severity level.
 * @param module Source location metadata.
 * @param kvs Key-value pairs.
 * @param fmt Format string.
 * @param args Format arguments.
 * @return Nothing.
 */
void log(Level level,
         std::source_location module,
         std::initializer_list<BasicKV<CharT>> kvs,
         const CharT* fmt,
         Args&&... args) {
    log(level, module, logger<CharT>(), basic_target_t<CharT>{}, kvs, fmt,
        std::forward<Args>(args)...);
}

template <typename CharT, typename... Args>
/**
 * @brief Overload of `log()` using global logger without target and key-value
 * pairs.
 *
 * Prefer using the `LOG_PP_*` macros for automatic source location capture.
 *
 * Example:
 * @code
 * LOG_INFO("done {}", 42);
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
         const CharT* fmt,
         Args&&... args) {
    log(level, module, logger<CharT>(), basic_target_t<CharT>{}, {}, fmt,
        std::forward<Args>(args)...);
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
