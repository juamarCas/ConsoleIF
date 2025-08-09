cmake_minimum_required(VERSION 3.22)

set(UTILS_LIB_NAME Utils)
set(EXECUTABLE_NAME ConsoleIF)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/")

include(FetchContent)

project(ConsoleIF
        VERSION 0.1.0
        LANGUAGES C CXX)
        
set(CMAKE_CXX_STANDARD 20)

option(DEBUG_MODE "Enable debug mode (sets DEBUG=1)" ON)
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
FetchContent_MakeAvailable(spdlog)

add_subdirectory(configured)
add_subdirectory(external)
add_subdirectory(Includes)


add_library(ConsoleIF INTERFACE)


target_include_directories(ConsoleIF INTERFACE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Includes>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/configured_files/include>
    $<INSTALL_INTERFACE:include>
)


target_link_libraries(ConsoleIF INTERFACE
    spdlog::spdlog
)

