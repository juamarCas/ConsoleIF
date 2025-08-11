cmake_minimum_required(VERSION 3.22)

set(UTILS_LIB_NAME Utils)
set(EXECUTABLE_NAME ConsoleIF)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/")
set(TEST_EXECUTABLE_NAME ConsoleIfTests)

include(FetchContent)

project(ConsoleIF
        VERSION 0.1.0
        LANGUAGES C CXX)
        
set(CMAKE_CXX_STANDARD 20)

option(DEBUG_MODE "Enable debug mode (sets DEBUG=1)" OFF)
option(BUILD_TESTS "Build tests" OFF)

if(DEBUG_MODE)
    add_compile_definitions(DEBUG=1)
else()
    add_compile_definitions(DEBUG=0)
endif()

FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.15.3
        GIT_SHALLOW TRUE
)

if(BUILD_TESTS)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.17.0
        GIT_SHALLOW TRUE
    )

    FetchContent_GetProperties(googletest)
    if(NOT googletest_POPULATED)
        FetchContent_Populate(googletest)
        add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)
    endif()
endif()

FetchContent_MakeAvailable(spdlog)



add_subdirectory(configured)
add_subdirectory(external)
add_subdirectory(src)
add_subdirectory(Includes)

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()

add_library(ConsoleIF INTERFACE)


target_include_directories(ConsoleIF INTERFACE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Includes>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/configured_files/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(ConsoleIF INTERFACE
    spdlog::spdlog
)


 


