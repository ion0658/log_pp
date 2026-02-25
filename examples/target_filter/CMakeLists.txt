add_executable(target_filter)

log_pp_set_compiler_options(target_filter)
log_pp_copy_dependency_dlls(target_filter)

target_sources(
    target_filter
    PRIVATE
    main.cpp
)

target_link_libraries(
    target_filter
    PRIVATE
    log_pp
)

