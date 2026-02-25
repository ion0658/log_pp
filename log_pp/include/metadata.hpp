#pragma once

#include <string_view>

#include "level.hpp"

#ifndef __LOG_PP_METADATA_HPP__
#define __LOG_PP_METADATA_HPP__

namespace log_pp {

/**
 * @brief Lightweight metadata used for logger-side filtering.
 *
 * Example:
 * @code
 * auto md = log_pp::MetadataBuilder{}
 *               .set_level(log_pp::Level::Info)
 *               .set_target("api")
 *               .build();
 * @endcode
 *
 * @tparam CharT Character type.
 */
template <typename CharT>
struct BasicMetadata {
    Level level{};
    std::basic_string_view<CharT> target{};

    /** @brief Returns the log level. @return Log level. */
    Level get_level() const noexcept;
    /** @brief Returns the target/category. @return Target/category text. */
    std::basic_string_view<CharT> get_target() const noexcept;
};

/**
 * @brief Builder for @ref BasicMetadata.
 *
 * @tparam CharT Character type.
 */
template <typename CharT>
struct BasicMetadataBuilder {
    BasicMetadata<CharT> metadata{};

    /**
     * @brief Sets log level.
     * @param level Log level.
     * @return This builder.
     */
    BasicMetadataBuilder& set_level(const Level level) noexcept;
    /**
     * @brief Sets log target/category.
     * @param target Target/category text.
     * @return This builder.
     */
    BasicMetadataBuilder& set_target(
        const std::basic_string_view<CharT> target) noexcept;

    /** @brief Returns an immutable metadata snapshot. @return Built metadata
     * value. */
    BasicMetadata<CharT> build() const noexcept;
};

/** @brief UTF-8 metadata alias. */
using Metadata = BasicMetadata<char>;
/** @brief UTF-8 metadata builder alias. */
using MetadataBuilder = BasicMetadataBuilder<char>;

template <typename CharT>
Level BasicMetadata<CharT>::get_level() const noexcept {
    return this->level;
}

template <typename CharT>
std::basic_string_view<CharT> BasicMetadata<CharT>::get_target()
    const noexcept {
    return this->target;
}

template <typename CharT>
BasicMetadataBuilder<CharT>& BasicMetadataBuilder<CharT>::set_level(
    const Level level) noexcept {
    this->metadata.level = level;
    return *this;
}

template <typename CharT>
BasicMetadataBuilder<CharT>& BasicMetadataBuilder<CharT>::set_target(
    const std::basic_string_view<CharT> target) noexcept {
    this->metadata.target = target;
    return *this;
}

template <typename CharT>
BasicMetadata<CharT> BasicMetadataBuilder<CharT>::build() const noexcept {
    return this->metadata;
}

}  // namespace log_pp

#endif  // !__LOG_PP_METADATA_HPP__
