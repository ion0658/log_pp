function(log_pp_copy_dependency_dlls target_name)
    if(WIN32 AND BUILD_SHARED_LIBS)
        add_custom_command(
            TARGET ${target_name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_RUNTIME_DLLS:${target_name}>
                $<TARGET_FILE_DIR:${target_name}>
            COMMAND_EXPAND_LISTS
            VERBATIM
            COMMENT "Copying DLL dependencies for ${target_name}"
        )
    endif()
endfunction()

function(log_pp_set_compiler_warnings target_name)
    target_compile_options(
        ${target_name}
        PRIVATE
            $<$<CXX_COMPILER_ID:MSVC>:
                /W4 /MP /utf-8 /WX>
            $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:
                -Wall -Wextra -pedantic -Werror>
    )
endfunction()

function(log_pp_set_export_directory target_name)
    if(PROJECT_IS_TOP_LEVEL)
        # set the output directories for the target
        set_target_properties(${target_name} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY
                ${PROJECT_BINARY_DIR}/bin/${target_name}/$<CONFIG>
            LIBRARY_OUTPUT_DIRECTORY
                ${PROJECT_BINARY_DIR}/lib/${target_name}/$<CONFIG>
            ARCHIVE_OUTPUT_DIRECTORY
                ${PROJECT_BINARY_DIR}/lib/${target_name}/$<CONFIG>
        )
    endif()
endfunction()

function(log_pp_set_compiler_options target_name)
    log_pp_set_compiler_warnings(${target_name})

    # set the C++ standard to C++20
    target_compile_features(${target_name} PRIVATE cxx_std_20)
    log_pp_set_export_directory(${target_name})
endfunction()

function(log_pp_create_test target_name)
    add_executable(${target_name})
    log_pp_set_compiler_options(${target_name})
    target_link_libraries(
        ${target_name}
        PRIVATE
        gtest_main
        gmock
        log_pp
    )
    target_sources(
        ${target_name}
        PRIVATE
        ${target_name}.cpp
    )

    add_test(NAME ${target_name} COMMAND ${target_name})
    log_pp_copy_dependency_dlls(${target_name})
endfunction()

