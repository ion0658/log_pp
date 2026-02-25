add_executable(comptime_filter)

log_pp_set_compiler_options(comptime_filter)
log_pp_copy_dependency_dlls(comptime_filter)

target_sources(
    comptime_filter
    PRIVATE
    main.cpp
)

target_link_libraries(
    comptime_filter
    PRIVATE
    log_pp
)

target_compile_definitions(
    comptime_filter
    PRIVATE
    LOG_PP_LEVEL_FILTER_DEBUG
    LOG_PP_RELEASE_LEVEL_FILTER_INFO
)

