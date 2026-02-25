#pragma once

#include "level.hpp"
#ifndef __LOG_PP_COMPTILE_FILTER_HPP__
#define __LOG_PP_COMPTILE_FILTER_HPP__

namespace log_pp {
constexpr Level get_comptime_level() noexcept {
#if defined(NDEBUG) && defined(LOG_PP_RELEASE_LEVEL_FILTER_ERROR)
    return Level::Error;

#elif defined(NDEBUG) && defined(LOG_PP_RELEASE_LEVEL_FILTER_WARN)
    return Level::Warning;

#elif defined(NDEBUG) && defined(LOG_PP_RELEASE_LEVEL_FILTER_INFO)
    return Level::Info;

#elif defined(NDEBUG) && defined(LOG_PP_RELEASE_LEVEL_FILTER_DEBUG)
    return Level::Debug;

#elif defined(NDEBUG) && defined(LOG_PP_RELEASE_LEVEL_FILTER_TRACE)
    return Level::Trace;

#elif defined(LOG_PP_LEVEL_FILTER_ERROR)
    return Level::Error;

#elif defined(LOG_PP_LEVEL_FILTER_WARN)
    return Level::Warning;

#elif defined(LOG_PP_LEVEL_FILTER_INFO)
    return Level::Info;

#elif defined(LOG_PP_LEVEL_FILTER_DEBUG)
    return Level::Debug;

#else
    return Level::Trace;

#endif
}
}  // namespace log_pp

#endif  // !__LOG_PP_COMPTILE_FILTER_HPP__
