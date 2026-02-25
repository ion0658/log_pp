#pragma once

#include <concepts>
#include <format>
#include <functional>
#include <iterator>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#ifndef __LOG_PP_KV_HPP__
#define __LOG_PP_KV_HPP__

namespace log_pp {

namespace detail {

template <typename CharT>
std::basic_string<CharT> default_kv_value_format() {
    if constexpr (std::same_as<CharT, char>) {
        return std::string("{}");
    } else if constexpr (std::same_as<CharT, wchar_t>) {
        return std::wstring(L"{}");
    } else {
        return std::basic_string<CharT>({CharT('{'), CharT('}')});
    }
}

template <typename CharT, typename T>
std::basic_string<CharT> format_kv_value(T&& value,
                                         std::basic_string_view<CharT> format) {
    if constexpr (std::same_as<CharT, char>) {
        return std::vformat(format, std::make_format_args(value));
    } else if constexpr (std::same_as<CharT, wchar_t>) {
        return std::vformat(format, std::make_wformat_args(value));
    } else {
        auto store = std::make_format_args<std::basic_format_context<
            std::back_insert_iterator<std::basic_string<CharT>>, CharT>>(value);
        return std::vformat(
            format,
            std::basic_format_args<std::basic_format_context<
                std::back_insert_iterator<std::basic_string<CharT>>, CharT>>(
                store));
    }
}

template <typename CharT, typename T>
std::basic_string<CharT> format_kv_value(T&& value) {
    return format_kv_value<CharT>(std::forward<T>(value),
                                  default_kv_value_format<CharT>());
}

template <typename T>
auto make_kv_value_holder(T&& value) {
    if constexpr (std::is_lvalue_reference_v<T>) {
        return std::cref(value);
    } else {
        return std::decay_t<T>(std::forward<T>(value));
    }
}

template <typename T>
decltype(auto) unwrap_kv_value_holder(const std::reference_wrapper<T>& value) {
    return value.get();
}

template <typename T>
decltype(auto) unwrap_kv_value_holder(const T& value) {
    return value;
}

}  // namespace detail

template <typename CharT>
/**
 * @brief Key-value pair stored in a log record.
 *
 * Values are formatted lazily on access.
 *
 * Example:
 * @code
 * log_pp::KV id{"request_id", 42};
 * log_pp::KV hex{"id", 255, "0x{:04X}"};
 * @endcode
 *
 * @tparam CharT Character type.
 */
struct BasicKV {
   private:
    std::basic_string_view<CharT> key;
    std::function<std::basic_string<CharT>()> value_formatter;

   public:
    BasicKV() = default;

    /**
     * @brief Creates a key-value pair using the default format string (`{}`).
     *
     * @tparam T Value type.
     * @param in_key Key text.
     * @param in_value Value to be formatted and stored.
     */
    template <typename T>
    BasicKV(std::basic_string_view<CharT> in_key, T&& in_value) : key(in_key) {
        auto stored_value =
            detail::make_kv_value_holder<T>(std::forward<T>(in_value));
        value_formatter = [stored_value = std::move(stored_value)]() {
            return detail::format_kv_value<CharT>(
                detail::unwrap_kv_value_holder(stored_value));
        };
    }

    /**
     * @brief Creates a key-value pair using a custom value format string.
     *
     * @tparam T Value type.
     * @param in_key Key text.
     * @param in_value Value to be formatted and stored.
     * @param in_format Format string for value.
     */
    template <typename T>
    BasicKV(std::basic_string_view<CharT> in_key,
            T&& in_value,
            std::basic_string_view<CharT> in_format)
        : key(in_key) {
        auto stored_value =
            detail::make_kv_value_holder<T>(std::forward<T>(in_value));
        auto format = std::basic_string<CharT>(in_format);
        value_formatter = [stored_value = std::move(stored_value),
                           format = std::move(format)]() {
            return detail::format_kv_value<CharT>(
                detail::unwrap_kv_value_holder(stored_value), format);
        };
    }

    /**
     * @brief Convenience constructor with C-string key.
     *
     * @tparam T Value type.
     * @param in_key Key text.
     * @param in_value Value to be formatted and stored.
     */
    template <typename T>
    BasicKV(const CharT* in_key, T&& in_value)
        : BasicKV(std::basic_string_view<CharT>(in_key),
                  std::forward<T>(in_value)) {}

    /**
     * @brief Convenience constructor with C-string key and format.
     *
     * @tparam T Value type.
     * @param in_key Key text.
     * @param in_value Value to be formatted and stored.
     * @param in_format Format string for value.
     */
    template <typename T>
    BasicKV(const CharT* in_key, T&& in_value, const CharT* in_format)
        : BasicKV(std::basic_string_view<CharT>(in_key),
                  std::forward<T>(in_value),
                  std::basic_string_view<CharT>(in_format)) {}

    /** @brief Returns the key. @return Key text. */
    std::basic_string_view<CharT> get_key() const noexcept { return key; }
    /** @brief Returns the formatted value. @return Formatted value text. */
    std::basic_string<CharT> get_value() const noexcept {
        return value_formatter();
    }
};

/**
 * @brief Container for heterogeneous key-value pairs.
 *
 * @tparam CharT Character type.
 */
template <typename CharT>
using BasicKVList = std::vector<BasicKV<CharT>>;

/** @brief UTF-8 key-value pair alias. */
using KV = BasicKV<char>;
/** @brief UTF-8 key-value list alias. */
using KVList = BasicKVList<char>;

}  // namespace log_pp

#endif  // !__LOG_PP_KV_HPP__
