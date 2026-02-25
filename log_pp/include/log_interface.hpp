#pragma once

#include "metadata.hpp"
#include "record.hpp"

#ifndef __LOG_PP_LOG_INTERFACE_HPP__
#define __LOG_PP_LOG_INTERFACE_HPP__

namespace log_pp {

/**
 * @brief Logger interface consumed by log_pp.
 *
 * Implementations receive structured records and can decide whether a record
 * should be emitted by overriding @ref enabled.
 *
 * Example:
 * @code
 * struct MyLogger : log_pp::BasicLogger<char> {
 *   bool enabled(const log_pp::BasicMetadata<char>&) const noexcept override {
 *     return true;
 *   }
 *   void log(const log_pp::BasicRecord<char>&) override {}
 *   void flush() override {}
 * };
 * @endcode
 *
 * @tparam CharT Character type.
 */
template <typename CharT>
struct BasicLogger {
    constexpr virtual ~BasicLogger() noexcept = default;

    /**
     * @brief Returns whether a log with the given metadata should be emitted.
     *
     * Example: filter by target/category.
     *
     * @param metadata Metadata used for filtering.
     * @return `true` if the record should be logged.
     */
    virtual bool enabled(
        const BasicMetadata<CharT>& metadata) const noexcept = 0;

    /**
     * @brief Emits one structured log record.
     *
     * Example: format with `std::vformat(record.get_format_string(),
     * record.get_args())`.
     *
     * @param record Structured record to emit.
     * @return Nothing.
     */
    virtual void log(const BasicRecord<CharT>& record) = 0;
    /**
     * @brief Flushes buffered output.
     *
     * @return Nothing.
     */
    virtual void flush() = 0;
};

/** @brief UTF-8 logger interface alias. */
using ILogger = BasicLogger<char>;

}  // namespace log_pp

#endif  // !__LOG_PP_LOG_INTERFACE_HPP__
