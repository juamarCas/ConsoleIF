cmake_minimum_required(VERSION 3.22)

set(UTILS_LIB_NAME Utils)
set(EXECUTABLE_NAME ConsoleIF)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/")



include(FetchContent)

project(ConsoleIF
        VERSION 0.0.1
        LANGUAGES C CXX)

FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.15.3
        GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(spdlog)

add_subdirectory(configured)
add_subdirectory(external)
add_subdirectory(src)
add_subdirectory(Includes)

set(INPUT_SOURCE "main.cpp")
add_executable(${EXECUTABLE_NAME} ${INPUT_SOURCE})

target_include_directories(${EXECUTABLE_NAME} PUBLIC 
        "${CMAKE_BINARY_DIR/configured_files}")


target_link_libraries(${EXECUTABLE_NAME} PUBLIC ${UTILS_LIB_NAME}
                                                spdlog::spdlog)
