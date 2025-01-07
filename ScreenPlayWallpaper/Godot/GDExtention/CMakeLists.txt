cmake_minimum_required(VERSION 3.22)

# Main project information
project(ScreenPlayGodotWallpaper LANGUAGES CXX)
find_package(Catch2 CONFIG REQUIRED)

set(SOURCES
    src/ScreenPlayGodotWallpaper.h
    src/ScreenPlayGodotWallpaper.cpp
    src/WindowsPipe.h
    src/WindowsPipe.cpp
    src/RegisterExtension.cpp)

# Create our library
add_library(${PROJECT_NAME} STATIC ${SOURCES})

if(WIN32)
    # Ensure both Debug and Release configurations use dynamic runtime
    set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

    # Set iterator debug level consistently
    target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:_ITERATOR_DEBUG_LEVEL=2>
                                                       $<$<CONFIG:Release>:_ITERATOR_DEBUG_LEVEL=0>)
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC godot::cpp ScreenPlayWallpaperLib)
target_include_directories(${PROJECT_NAME} PRIVATE "src")

# # Runs after compilation Enable executing python scripts find_package(Python COMPONENTS Interpreter)

# add_custom_command( TARGET ${PROJECT_NAME} POST_BUILD WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/Tools" COMMAND "${Python_EXECUTABLE}"
# "build_godot.py" "--build_path" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" COMMENT "Running post-build step")

add_subdirectory(src)
add_subdirectory(extern/godot-cpp)
