# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

add_subdirectory(GDExtention)

# Disable for ci because it uses separate build step.
# if(NOT ${GITLAB_CI} AND ${GODOT_WALLPAPER})
# # Normally I would directly call the Godot executable here but after hours of failure without any error messages, lets just call the
# # python script that we want to have anyway...
# find_package(Python COMPONENTS Interpreter)

# message(STATUS "GODOT_WORKING_DIRECTORY         = ${CMAKE_SOURCE_DIR}/Tools")
# message(STATUS "Generate Godot export:")
# message(STATUS "cd ${CMAKE_SOURCE_DIR}/Tools")
# message(STATUS "python build_godot.py --skip_if_exists --build_path '${CMAKE_RUNTIME_OUTPUT_DIRECTORY}'")

# execute_process(
# COMMAND ${Python_EXECUTABLE} build_godot.py --skip_if_exists --build_path "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
# WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/Tools"
# TIMEOUT 30
# RESULT_VARIABLE GODOT_EXPORT_RESULT
# OUTPUT_VARIABLE GODOT_EXPORT_OUTPUT ENCODING UTF8)

# if(GODOT_EXPORT_RESULT EQUAL "1")
# message(STATUS "Godot export skipped because it already exists.")
# elseif(
# NOT
# GODOT_EXPORT_RESULT
# EQUAL
# "0")
# message(STATUS "################################################")
# message(STATUS "Unable to export Map. Please run Tools/setup.py!")
# message(STATUS "################################################")
# message(FATAL_ERROR "ERROR: ${GODOT_EXPORT_OUTPUT}")
# endif()
# endif()
