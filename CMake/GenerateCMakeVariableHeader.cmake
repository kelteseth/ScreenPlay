#! generate_cmake_variable_header : Generates a header CmakeVariables.h that contains defines for the variables specified in CmakeVariables.h.in!
#
# The generated CmakeVariables.h header can then be used to access e.g. the PROJECT_NAME define in C++ code.
#
# Example
# generate_cmake_variable_header(${PROJECT_NAME})
#
function(generate_cmake_variable_header TARGET)
    set(SOURCE_DIR ${CMAKE_SOURCE_DIR})
    set(SCREENPLAY_GODOT_VERSION ${GODOT_VERSION})

    # Specify the configuration file from which the header file will be generated
    configure_file(${CMAKE_SOURCE_DIR}/CMake/CMakeVariables.h.in ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/CMakeVariables.h @ONLY)
    message(STATUS "GENERATE: ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/CMakeVariables.h and add ${TARGET} to ${CMAKE_CURRENT_BINARY_DIR}")
    # Add the directory containing the generated header
    target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
endfunction()
