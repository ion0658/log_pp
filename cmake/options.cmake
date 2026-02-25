option(BUILD_TESTING "Build testing" ${PROJECT_IS_TOP_LEVEL})
if (BUILD_TESTING)
    enable_testing()
    include(CTest)
endif()

option(BUILD_EXAMPLES "Build examples" ${PROJECT_IS_TOP_LEVEL})

option(LOG_PP_LEVEL_FILTER_TRACE "compile time log level filter with trace" OFF)
option(LOG_PP_LEVEL_FILTER_DEBUG "compile time log level filter with debug" OFF)
option(LOG_PP_LEVEL_FILTER_INFO "compile time log level filter with info" OFF)
option(LOG_PP_LEVEL_FILTER_WARN "compile time log level filter with warn" OFF)
option(LOG_PP_LEVEL_FILTER_ERROR "compile time log level filter with error" OFF)

option(LOG_PP_RELEASE_LEVEL_FILTER_TRACE "compile time log level filter with trace in release mode" OFF)
option(LOG_PP_RELEASE_LEVEL_FILTER_DEBUG "compile time log level filter with debug in release mode" OFF)
option(LOG_PP_RELEASE_LEVEL_FILTER_INFO "compile time log level filter with info in release mode" OFF)
option(LOG_PP_RELEASE_LEVEL_FILTER_WARN "compile time log level filter with warn in release mode" OFF)
option(LOG_PP_RELEASE_LEVEL_FILTER_ERROR "compile time log level filter with error in release mode" OFF)

set(_log_pp_level_filter_non_release_defines "")
if(LOG_PP_LEVEL_FILTER_TRACE)
    list(APPEND _log_pp_level_filter_non_release_defines LOG_PP_LEVEL_FILTER_TRACE)
endif()
if(LOG_PP_LEVEL_FILTER_DEBUG)
    list(APPEND _log_pp_level_filter_non_release_defines LOG_PP_LEVEL_FILTER_DEBUG)
endif()
if(LOG_PP_LEVEL_FILTER_INFO)
    list(APPEND _log_pp_level_filter_non_release_defines LOG_PP_LEVEL_FILTER_INFO)
endif()
if(LOG_PP_LEVEL_FILTER_WARN)
    list(APPEND _log_pp_level_filter_non_release_defines LOG_PP_LEVEL_FILTER_WARN)
endif()
if(LOG_PP_LEVEL_FILTER_ERROR)
    list(APPEND _log_pp_level_filter_non_release_defines LOG_PP_LEVEL_FILTER_ERROR)
endif()

list(LENGTH _log_pp_level_filter_non_release_defines
     _log_pp_level_filter_non_release_count)
if(_log_pp_level_filter_non_release_count GREATER 1)
    message(
        FATAL_ERROR
            "Only one of LOG_PP_LEVEL_FILTER_TRACE/DEBUG/INFO/WARN/ERROR can be ON")
endif()

if(_log_pp_level_filter_non_release_count EQUAL 1)
    list(GET _log_pp_level_filter_non_release_defines 0
         LOG_PP_COMPILE_LEVEL_FILTER_DEFINE)
else()
    unset(LOG_PP_COMPILE_LEVEL_FILTER_DEFINE)
endif()

set(_log_pp_level_filter_release_defines "")
if(LOG_PP_RELEASE_LEVEL_FILTER_TRACE)
    list(APPEND _log_pp_level_filter_release_defines LOG_PP_RELEASE_LEVEL_FILTER_TRACE)
endif()
if(LOG_PP_RELEASE_LEVEL_FILTER_DEBUG)
    list(APPEND _log_pp_level_filter_release_defines LOG_PP_RELEASE_LEVEL_FILTER_DEBUG)
endif()
if(LOG_PP_RELEASE_LEVEL_FILTER_INFO)
    list(APPEND _log_pp_level_filter_release_defines LOG_PP_RELEASE_LEVEL_FILTER_INFO)
endif()
if(LOG_PP_RELEASE_LEVEL_FILTER_WARN)
    list(APPEND _log_pp_level_filter_release_defines LOG_PP_RELEASE_LEVEL_FILTER_WARN)
endif()
if(LOG_PP_RELEASE_LEVEL_FILTER_ERROR)
    list(APPEND _log_pp_level_filter_release_defines LOG_PP_RELEASE_LEVEL_FILTER_ERROR)
endif()

list(LENGTH _log_pp_level_filter_release_defines
     _log_pp_level_filter_release_count)
if(_log_pp_level_filter_release_count GREATER 1)
    message(
        FATAL_ERROR
            "Only one of LOG_PP_RELEASE_LEVEL_FILTER_TRACE/DEBUG/INFO/WARN/ERROR can be ON")
endif()

if(_log_pp_level_filter_release_count EQUAL 1)
    list(GET _log_pp_level_filter_release_defines 0
         LOG_PP_COMPILE_RELEASE_LEVEL_FILTER_DEFINE)
else()
    unset(LOG_PP_COMPILE_RELEASE_LEVEL_FILTER_DEFINE)
endif()

