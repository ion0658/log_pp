log_pp_create_test(level_test)
log_pp_create_test(level_filter_test)
log_pp_create_test(macro_test)
log_pp_create_test(kv_test)

log_pp_create_test(compile_filter_test)
target_compile_definitions(
    compile_filter_test
    PRIVATE
    LOG_PP_LEVEL_FILTER_INFO
)

log_pp_create_test(compile_filter_release_fallback_test)
target_compile_definitions(
    compile_filter_release_fallback_test
    PRIVATE
    NDEBUG
    LOG_PP_LEVEL_FILTER_INFO
)

