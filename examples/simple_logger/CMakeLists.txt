add_executable(simple_logger)

log_pp_set_compiler_options(simple_logger)
log_pp_copy_dependency_dlls(simple_logger)

target_sources(
    simple_logger
    PRIVATE
    main.cpp
)

target_link_libraries(
    simple_logger
    PRIVATE
    log_pp
)

