# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
function(check_and_build_godot EXPORT_PATH)
    if(NOT ${SCREENPLAY_GODOT})
        return()
    endif()

    # Determine the executable name based on platform
    if(WIN32)
        set(GODOT_EXECUTABLE_NAME "ScreenPlayWallpaperGodot.exe")
    else()
        set(GODOT_EXECUTABLE_NAME "ScreenPlayWallpaperGodot")
    endif()

    # Define possible build paths to check
    set(BUILD_PATHS_TO_CHECK)

    if(WIN32)
        list(APPEND BUILD_PATHS_TO_CHECK
            "${CMAKE_SOURCE_DIR}/Build/MSVC_Release/ScreenPlay/${GODOT_EXECUTABLE_NAME}"
            "${CMAKE_SOURCE_DIR}/Build/MSVC_Debug/ScreenPlay/${GODOT_EXECUTABLE_NAME}"
            "${CMAKE_SOURCE_DIR}/Build/Clang_Release/ScreenPlay/${GODOT_EXECUTABLE_NAME}"
            "${CMAKE_SOURCE_DIR}/Build/Clang_Debug/ScreenPlay/${GODOT_EXECUTABLE_NAME}")
    elseif(APPLE)
        list(APPEND BUILD_PATHS_TO_CHECK
            "${CMAKE_SOURCE_DIR}/Build/Clang_Release/ScreenPlay/${GODOT_EXECUTABLE_NAME}"
            "${CMAKE_SOURCE_DIR}/Build/Clang_Debug/ScreenPlay/${GODOT_EXECUTABLE_NAME}")
    elseif(UNIX)
        list(APPEND BUILD_PATHS_TO_CHECK
            "${CMAKE_SOURCE_DIR}/Build/Gcc_Release/ScreenPlay/${GODOT_EXECUTABLE_NAME}"
            "${CMAKE_SOURCE_DIR}/Build/Gcc_Debug/ScreenPlay/${GODOT_EXECUTABLE_NAME}"
            "${CMAKE_SOURCE_DIR}/Build/Clang_Release/ScreenPlay/${GODOT_EXECUTABLE_NAME}"
            "${CMAKE_SOURCE_DIR}/Build/Clang_Debug/ScreenPlay/${GODOT_EXECUTABLE_NAME}")
    endif()

    # Also check the specified export path
    list(APPEND BUILD_PATHS_TO_CHECK "${EXPORT_PATH}/${GODOT_EXECUTABLE_NAME}")

    # Check if executable exists in any of the paths
    set(GODOT_EXECUTABLE_FOUND FALSE)

    foreach(BUILD_PATH ${BUILD_PATHS_TO_CHECK})
        if(EXISTS "${BUILD_PATH}")
            set(GODOT_EXECUTABLE_FOUND TRUE)
            message(STATUS "Found ScreenPlayWallpaperGodot at: ${BUILD_PATH}")
            break()
        endif()
    endforeach()

    # If not found, run the build script
    if(NOT GODOT_EXECUTABLE_FOUND)
        message(STATUS "ScreenPlayWallpaperGodot executable not found, running build script...")

        find_package(Python3 REQUIRED)

        set(BUILD_GODOT_SCRIPT "${CMAKE_SOURCE_DIR}/Tools/build_godot.py")

        if(EXISTS "${BUILD_GODOT_SCRIPT}")
            execute_process(
                COMMAND ${Python3_EXECUTABLE} "${BUILD_GODOT_SCRIPT}" --build_path "${EXPORT_PATH}"
                WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
                RESULT_VARIABLE BUILD_RESULT
                OUTPUT_VARIABLE BUILD_OUTPUT
                ERROR_VARIABLE BUILD_ERROR
            )

            if(BUILD_RESULT EQUAL 0)
                message(STATUS "Successfully built ScreenPlayWallpaperGodot")
            else()
                message(WARNING "Failed to build ScreenPlayWallpaperGodot: ${BUILD_ERROR}")
            endif()
        else()
            message(WARNING "Build script not found at: ${BUILD_GODOT_SCRIPT}")
        endif()
    endif()
endfunction()