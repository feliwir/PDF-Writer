project(pdfwriter)
cmake_minimum_required (VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules" ${CMAKE_MODULE_PATH})

option(BUILD_COVERAGE "Build coverage" ON)
option(BUILD_TESTING "Build tests" ON)

if(BUILD_COVERAGE)
    include(CodeCoverage)
    append_coverage_compiler_flags()
endif()

add_subdirectory(contrib)
add_subdirectory(src)
add_subdirectory(demo)

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()