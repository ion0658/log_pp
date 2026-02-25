cmake_minimum_required(VERSION 3.15)

project(log_pp VERSION 0.1.0 LANGUAGES CXX)

include(GenerateExportHeader)

include(cmake/configure.cmake)
include(cmake/options.cmake)
include(cmake/functions.cmake)

add_subdirectory(log_pp)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

add_subdirectory(3rdparty)

