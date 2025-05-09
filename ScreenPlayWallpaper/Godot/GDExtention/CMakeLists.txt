cmake_minimum_required(VERSION 3.22)

# Main project information
project(ScreenPlayGodotWallpaper LANGUAGES CXX)
find_package(Catch2 CONFIG REQUIRED)
find_package(unofficial-godot-cpp CONFIG REQUIRED)

set(SOURCES
    src/ScreenPlayGodotWallpaper.h
    src/ScreenPlayGodotWallpaper.cpp
    src/WindowsPipe.h
    src/WindowsPipe.cpp
    src/RegisterExtension.cpp)

# Create our library
add_library(${PROJECT_NAME} SHARED ${SOURCES})

if(WIN32)
    # Ensure both Debug and Release configurations use dynamic runtime
    set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

    # Set iterator debug level consistently
    target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:_ITERATOR_DEBUG_LEVEL=2>
        $<$<CONFIG:Release>:_ITERATOR_DEBUG_LEVEL=0>)
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC unofficial::godot::cpp ScreenPlayWallpaperWindowsIntegration )
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC ScreenPlayCoreWindowsExtrasLib)
endif()
target_include_directories(${PROJECT_NAME} PRIVATE "src")

set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")

file(REAL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../" SPGodot_BASE_PATH EXPAND_TILDE)
set(OUT_BASE_PATH "${SPGodot_BASE_PATH}/Godot/ScreenPlayGodot/ScreenPlayGodotWallpaper/lib/")

if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${OUT_BASE_PATH}/Windows-AMD64"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUT_BASE_PATH}/Windows-AMD64"
        LIBRARY_OUTPUT_DIRECTORY_DEBUG "${OUT_BASE_PATH}/Windows-AMD64"
        LIBRARY_OUTPUT_DIRECTORY_RELEASE "${OUT_BASE_PATH}/Windows-AMD64"
    )
else()
    set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${OUT_BASE_PATH}/Linux-x86_64"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUT_BASE_PATH}/Linux-x86_64"
        LIBRARY_OUTPUT_DIRECTORY_DEBUG "${OUT_BASE_PATH}/Linux-x86_64"
        LIBRARY_OUTPUT_DIRECTORY_RELEASE "${OUT_BASE_PATH}/Linux-x86_64"
    )
endif()

# To make it also work inside ScreenPlay dir
add_custom_command(TARGET ScreenPlayGodotWallpaper POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        "$<TARGET_FILE:ScreenPlayGodotWallpaper>"
        "${CMAKE_BINARY_DIR}/ScreenPlay/"
)

