#pragma once

#include <concepts>
#include <cstdint>
#include <format>
#include <iterator>
#include <optional>
#include <source_location>
#include <string>
#include <string_view>
#include <type_traits>

#include "kv.hpp"
#include "level.hpp"
#include "metadata.hpp"

#ifndef __LOG_PP_RECORD_HPP__
#define __LOG_PP_RECORD_HPP__

namespace log_pp {

/**
 * @brief Internal format context used for non-char/non-wchar_t character types.
 * @tparam CharT Character type.
 */
template <typename CharT>
using FormatContext = std::basic_format_context<
    std::back_insert_iterator<std::basic_string<CharT>>,
    CharT>;

/**
 * @brief Character-aware format argument wrapper used by records.
 * @tparam CharT Character type.
 */
template <typename CharT>
using FormatArgs = std::conditional_t<
    std::same_as<CharT, char>,
    std::format_args,
    std::conditional_t<std::same_as<CharT, wchar_t>,
                       std::wformat_args,
                       std::basic_format_args<FormatContext<CharT>>>>;

/**
 * @brief Creates an empty argument store for @ref FormatArgs.
 * @tparam CharT Character type.
 * @return Empty format argument store.
 */
template <typename CharT>
FormatArgs<CharT> make_empty_format_args() {
    if constexpr (std::same_as<CharT, char>) {
        return std::make_format_args();
    } else if constexpr (std::same_as<CharT, wchar_t>) {
        return std::make_wformat_args();
    } else {
        static auto store = std::make_format_args<FormatContext<CharT>>();
        return FormatArgs<CharT>(store);
    }
}

/**
 * @brief Structured log payload delivered to logger implementations.
 *
 * Example:
 * @code
 * auto rec = log_pp::RecordBuilder{}
 *               .set_level(log_pp::Level::Info)
 *               .set_target("api")
 *               .set_format_string("hello {}")
 *               .build();
 * @endcode
 *
 * @tparam CharT Character type.
 */
template <typename CharT>
struct BasicRecord {
    BasicMetadata<CharT> metadata{};

    std::basic_string<CharT> format_string{};
    FormatArgs<CharT> args = make_empty_format_args<CharT>();
    BasicKVList<CharT> kvs{};
    std::optional<std::source_location> module;

    /** @brief Returns metadata used for filtering/routing. @return Metadata
     * value. */
    BasicMetadata<CharT> get_metadata() const noexcept;
    /** @brief Returns the original format string. @return Format string. */
    std::basic_string_view<CharT> get_format_string() const noexcept;
    /** @brief Returns stored formatting arguments. @return Stored format
     * arguments. */
    FormatArgs<CharT> get_args() const noexcept;
    /** @brief Returns attached key-value fields. @return Key-value list
     * reference. */
    const BasicKVList<CharT>& get_kvs() const noexcept;
    /** @brief Convenience accessor for metadata level. @return Log level. */
    Level get_level() const noexcept;
    /** @brief Convenience accessor for metadata target. @return Target/category
     * text. */
    std::basic_string_view<CharT> get_target() const noexcept;
    /** @brief Returns function name when source location is set. @return
     * Function name or empty. */
    std::optional<std::string_view> get_module_path() const noexcept;
    /** @brief Returns file path when source location is set. @return File path
     * or empty. */
    std::optional<std::string_view> get_file() const noexcept;
    /** @brief Returns line number when source location is set. @return Line
     * number or empty. */
    std::optional<uint32_t> get_line() const noexcept;
};

/**
 * @brief Builder for @ref BasicRecord.
 *
 * Example:
 * @code
 * auto rec = log_pp::RecordBuilder{}
 *               .set_level(log_pp::Level::Debug)
 *               .set_target("worker")
 *               .build();
 * @endcode
 *
 * @tparam CharT Character type.
 */
template <typename CharT>
struct BasicRecordBuilder {
    BasicRecord<CharT> record{};

    constexpr BasicRecordBuilder() noexcept = default;
    constexpr BasicRecordBuilder(const BasicRecord<CharT>& rhs) noexcept
        : record({
              .metadata =
                  {
                      .level = rhs.metadata.level,
                      .target = rhs.metadata.target,
                  },
              .format_string = rhs.format_string,
              .args = rhs.args,
              .kvs = rhs.kvs,
              .module = rhs.module,
          }) {}

    /**
     * @brief Replaces metadata.
     * @param metadata Metadata value.
     * @return This builder.
     */
    BasicRecordBuilder& set_metadata(
        const BasicMetadata<CharT> metadata) noexcept;

    /**
     * @brief Sets level in metadata.
     * @param level Log level.
     * @return This builder.
     */
    BasicRecordBuilder& set_level(const Level level) noexcept;
    /**
     * @brief Sets target in metadata.
     * @param target Target/category text.
     * @return This builder.
     */
    BasicRecordBuilder& set_target(
        const std::basic_string_view<CharT> target) noexcept;
    /**
     * @brief Sets message format string.
     * @param format_string Format string text.
     * @return This builder.
     */
    BasicRecordBuilder& set_format_string(
        const std::basic_string_view<CharT> format_string) noexcept;
    /**
     * @brief Sets format arguments.
     * @param args Stored format arguments.
     * @return This builder.
     */
    BasicRecordBuilder& set_args(const FormatArgs<CharT> args) noexcept;
    /**
     * @brief Sets key-value fields.
     * @param kvs Key-value list.
     * @return This builder.
     */
    BasicRecordBuilder& set_kvs(const BasicKVList<CharT>& kvs) noexcept;
    /**
     * @brief Sets source location.
     * @param module Source location.
     * @return This builder.
     */
    BasicRecordBuilder& set_module(const std::source_location module) noexcept;

    /** @brief Returns an immutable record snapshot. @return Built record value.
     */
    BasicRecord<CharT> build() const noexcept;
};

/** @brief UTF-8 record alias. */
using Record = BasicRecord<char>;
/** @brief UTF-8 record builder alias. */
using RecordBuilder = BasicRecordBuilder<char>;

template <typename CharT>
BasicMetadata<CharT> BasicRecord<CharT>::get_metadata() const noexcept {
    return metadata;
}

template <typename CharT>
std::basic_string_view<CharT> BasicRecord<CharT>::get_format_string()
    const noexcept {
    return format_string;
}

template <typename CharT>
FormatArgs<CharT> BasicRecord<CharT>::get_args() const noexcept {
    return args;
}

template <typename CharT>
const BasicKVList<CharT>& BasicRecord<CharT>::get_kvs() const noexcept {
    return kvs;
}

template <typename CharT>
Level BasicRecord<CharT>::get_level() const noexcept {
    return metadata.level;
}

template <typename CharT>
std::basic_string_view<CharT> BasicRecord<CharT>::get_target() const noexcept {
    return metadata.target;
}

template <typename CharT>
std::optional<std::string_view> BasicRecord<CharT>::get_module_path()
    const noexcept {
    if (module.has_value()) {
        return module->function_name();
    }
    return std::nullopt;
}

template <typename CharT>
std::optional<std::string_view> BasicRecord<CharT>::get_file() const noexcept {
    if (module.has_value()) {
        return module->file_name();
    }
    return std::nullopt;
}

template <typename CharT>
std::optional<uint32_t> BasicRecord<CharT>::get_line() const noexcept {
    if (module.has_value()) {
        return module->line();
    }
    return std::nullopt;
}

template <typename CharT>
BasicRecordBuilder<CharT>& BasicRecordBuilder<CharT>::set_metadata(
    const BasicMetadata<CharT> metadata) noexcept {
    record.metadata = metadata;
    return *this;
}

template <typename CharT>
BasicRecordBuilder<CharT>& BasicRecordBuilder<CharT>::set_level(
    const Level level) noexcept {
    record.metadata.level = level;
    return *this;
}

template <typename CharT>
BasicRecordBuilder<CharT>& BasicRecordBuilder<CharT>::set_target(
    const std::basic_string_view<CharT> target) noexcept {
    record.metadata.target = target;
    return *this;
}

template <typename CharT>
BasicRecordBuilder<CharT>& BasicRecordBuilder<CharT>::set_format_string(
    const std::basic_string_view<CharT> format_string) noexcept {
    record.format_string = std::basic_string<CharT>(format_string);
    return *this;
}

template <typename CharT>
BasicRecordBuilder<CharT>& BasicRecordBuilder<CharT>::set_args(
    const FormatArgs<CharT> args) noexcept {
    record.args = args;
    return *this;
}

template <typename CharT>
BasicRecordBuilder<CharT>& BasicRecordBuilder<CharT>::set_kvs(
    const BasicKVList<CharT>& kvs) noexcept {
    record.kvs = kvs;
    return *this;
}

template <typename CharT>
BasicRecordBuilder<CharT>& BasicRecordBuilder<CharT>::set_module(
    const std::source_location module) noexcept {
    record.module = module;
    return *this;
}

template <typename CharT>
BasicRecord<CharT> BasicRecordBuilder<CharT>::build() const noexcept {
    return record;
}

}  // namespace log_pp

#endif  // !__LOG_PP_RECORD_HPP__
