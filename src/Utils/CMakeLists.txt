# Define the library name if not set
if(NOT DEFINED UTILS_LIB_NAME)
    set(UTILS_LIB_NAME "Utils")
endif()


add_library(${UTILS_LIB_NAME} STATIC "Utils.cpp")

target_include_directories(${UTILS_LIB_NAME} PUBLIC 
    "${CMAKE_SOURCE_DIR}/Includes/Utils"    # For Utils.h
)

target_link_libraries(${UTILS_LIB_NAME} PUBLIC Result)