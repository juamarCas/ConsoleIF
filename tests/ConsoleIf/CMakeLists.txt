
add_executable(${TEST_EXECUTABLE_NAME} consoleif_tests.cpp)
set_target_properties(${TEST_EXECUTABLE_NAME} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
target_link_libraries(${TEST_EXECUTABLE_NAME} PRIVATE   Result
                                                        Utils
                                                        ConsoleIf 
                                                        gtest_main 
                                                        gmock_main 
                                                        spdlog::spdlog)