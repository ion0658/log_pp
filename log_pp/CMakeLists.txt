add_library(log_pp)

log_pp_set_compiler_options(log_pp)

if(DEFINED LOG_PP_COMPILE_LEVEL_FILTER_DEFINE)
    target_compile_definitions(log_pp PUBLIC ${LOG_PP_COMPILE_LEVEL_FILTER_DEFINE})
endif()

if(DEFINED LOG_PP_COMPILE_RELEASE_LEVEL_FILTER_DEFINE)
    target_compile_definitions(log_pp PUBLIC ${LOG_PP_COMPILE_RELEASE_LEVEL_FILTER_DEFINE})
endif()

generate_export_header(log_pp)

target_include_directories(
    log_pp
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_include_directories(
    log_pp
    PRIVATE
        include_private
)
add_subdirectory(src)

