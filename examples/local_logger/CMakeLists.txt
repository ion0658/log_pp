add_executable(local_logger)

log_pp_set_compiler_options(local_logger)
log_pp_copy_dependency_dlls(local_logger)

target_sources(
    local_logger
    PRIVATE
    main.cpp
)

target_link_libraries(
    local_logger
    PRIVATE
    log_pp
)

