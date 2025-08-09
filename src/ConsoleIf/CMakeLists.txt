add_library(ConsoleIf STATIC "ConsoleIf.cpp")

target_include_directories(${UTILS_LIB_NAME} PUBLIC 
    "${CMAKE_SOURCE_DIR}/Includes/Utils"    
)

target_link_libraries(ConsoleIf PUBLIC Result
                                        spdlog::spdlog
                                        Utils)