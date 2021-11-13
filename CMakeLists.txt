cmake_minimum_required(VERSION 3.16.0)

project(ScreenPlay LANGUAGES CXX)

# This sets cmake to compile all dlls into the main directory
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

option(SCREENPLAY_STEAM  "For FOSS distribution so we do not bundle proprietary code." ON)
option(TESTS_ENABLED OFF)

# Add our *.cmake diretory to the CMAKE_MODULE_PATH, so that our includes are found
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})

file(MAKE_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})

set(CMAKE_CXX_STANDARD 20)

if(WIN32)
    set(VCPKG_ARCH "x64-windows")
elseif(UNIX AND NOT APPLE)
    set(VCPKG_ARCH "x64-linux")
elseif(APPLE)
    set(VCPKG_ARCH "x64-osx")
endif()

if(${TESTS_ENABLED})
    enable_testing()
endif()

set(VCPKG_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../ScreenPlay-vcpkg")
set(VCPKG_INSTALLED_PATH "${VCPKG_PATH}/installed/${VCPKG_ARCH}")

find_package(Git REQUIRED)
if(WIN32)
    set(date_command "CMD")
    set(date_arg "/c date /t")
elseif(APPLE)
    set(date_command "date")
    set(date_arg "")
elseif(UNIX)
    set(date_command "date")
    set(date_arg "")
endif()

execute_process(
    COMMAND ${date_command} ${date_arg}
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE BUILD_DATE
    OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(
    COMMAND "${GIT_EXECUTABLE}" describe --always
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE)

add_compile_definitions(COMPILE_INFO="Build Date: ${BUILD_DATE}. Git Hash: ${GIT_COMMIT_HASH}. ")
add_compile_definitions(SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
add_compile_definitions(SCREENPLAY_STEAM="${SCREENPLAY_STEAM}")

if(UNIX AND NOT APPLE)
    # Fixes QWebEngine linker errors on Ubuntu 20.04
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=gold")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=gold")
endif()

add_subdirectory(ScreenPlay)
add_subdirectory(ScreenPlaySDK)
add_subdirectory(ScreenPlayShader)
add_subdirectory(ScreenPlayWallpaper)
add_subdirectory(ScreenPlayWidget)
add_subdirectory(ScreenPlayUtil)
add_subdirectory(CMake)

if(${SCREENPLAY_STEAM})
    add_subdirectory(ScreenPlayWorkshop)
endif()

if(WIN32)
    add_subdirectory(ScreenPlaySysInfo)
endif()

message(STATUS "[DEFINE] SOURCE_DIR               = ${SOURCE_DIR}")
message(STATUS "[DEFINE] BUILD_DATE               = ${BUILD_DATE}")
message(STATUS "[DEFINE] GIT_COMMIT_HASH          = ${GIT_COMMIT_HASH}")
message(STATUS "[OPTION] SCREENPLAY_STEAM         = ${SCREENPLAY_STEAM}")
message(STATUS "[OPTION] TESTS_ENABLED            = ${TESTS_ENABLED}")
message(STATUS "[PROJECT] CMAKE_TOOLCHAIN_FILE    = ${CMAKE_TOOLCHAIN_FILE}")
message(STATUS "[PROJECT] VCPKG_PATH              = ${VCPKG_PATH}")
message(STATUS "[PROJECT] VCPKG_ARCH              = ${VCPKG_ARCH}")
message(STATUS "[PROJECT] CMAKE_PREFIX_PATH       = ${CMAKE_PREFIX_PATH}")
message(STATUS "[PROJECT] VCPKG_TARGET_TRIPLET    = ${VCPKG_TARGET_TRIPLET}")
