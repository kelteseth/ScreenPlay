cmake_minimum_required(VERSION 3.16.0)

project(
    ScreenPlay
    VERSION 0.15.0
    DESCRIPTION "Modern, Cross Plattform, Live Wallpaper, Widgets and AppDrawer!"
    HOMEPAGE_URL "https://screen-play.app/"
    LANGUAGES CXX)

# Only used for some custom file copy
if(WIN32)
    set(VCPKG_ARCH "x64-windows")
elseif(UNIX AND NOT APPLE)
    set(VCPKG_ARCH "x64-linux")
elseif(APPLE)
    # CMAKE_HOST_SYSTEM_PROCESSOR returns the value of `uname -p` on host. Universal builds are not available with vcpkg, but the prebuild
    # Qt binaries are. CMake also handles https://github.com/microsoft/vcpkg/discussions/19454 This means we must compile twice and then
    # merge the binaries with lipo
    if(CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL x86_64 OR CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL amd64)
        set(VCPKG_ARCH "x64-osx")
        set(CMAKE_OSX_ARCHITECTURES "x86_64")
        set(VCPKG_TARGET_ARCHITECTURE x86_64)
    elseif(CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL arm)
        set(VCPKG_ARCH "arm64-osx")
        set(CMAKE_OSX_ARCHITECTURES "arm64")
        set(VCPKG_TARGET_ARCHITECTURE arm64)
    endif()

    message(STATUS "[PROJECT] CMAKE_OSX_ARCHITECTURES      = ${CMAKE_OSX_ARCHITECTURES}")
endif()

# This sets cmake to compile all dlls into the main directory
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})

# This is needed for OSX: Because we bundle ScreenPlay and ScreenPlayWallpaper into .app they both need other QML dependencies like
# ScreenPlayUtil. The fastest way is to use a shared QML module path for development and add this path to the qml engines import pah. For
# the SCREENPLAY_DEPLOY we copy them into the matching dirs via the build.py
#
# This subdirectoy is needed for OSX and Linux to fix linker errors because we would have ScreenPlayApp executable and folder for the qml
# files in the same directory.
set(SCREENPLAY_QML_MODULES_PATH "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/qml")
# Adds the qml import path so QtCreator can find them
list(APPEND QML_DIRS "${SCREENPLAY_QML_MODULES_PATH}")
set(QML_IMPORT_PATH
    "${QML_DIRS}"
    CACHE STRING "Qt Creator extra qml import paths")

set(VCPKG_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../ScreenPlay-vcpkg")
set(VCPKG_INSTALLED_PATH "${VCPKG_PATH}/installed/${VCPKG_ARCH}")
set(VCPKG_BIN_PATH "${VCPKG_INSTALLED_PATH}/bin")

option(SCREENPLAY_STEAM "For FOSS distribution so we do not bundle proprietary code." ON)
option(SCREENPLAY_DEPLOY "Marks this version as an official deploy version. This version uses different import paths and other settings."
       OFF)
option(SCREENPLAY_TESTS "Enables UI tests." ON)
option(SCREENPLAY_INSTALLER "Indicates whether an installer via the Qt Installer Framework is created." OFF)

# Gitlab CI has many ENV variables. We use this one to check if the current build happens inside the CI
if(DEFINED ENV{CI_COMMIT_MESSAGE})
    set(GITLAB_CI true)
endif()

file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_compile_definitions(COMPILE_INFO="Build Date: ${BUILD_DATE}. Git Hash: ${GIT_COMMIT_HASH}. ")
add_compile_definitions(SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
if(${SCREENPLAY_DEPLOY})
    add_compile_definitions(DEPLOY_VERSION)
endif()

find_package(Git REQUIRED)
if(WIN32)
    set(DATE_COMMAND "CMD")
    set(DATE_ARG "/c date /t")
elseif(APPLE)
    set(DATE_COMMAND "date")
    set(DATE_ARG " +%y.%m.%d")
elseif(UNIX)
    set(DATE_COMMAND "date")
    set(DATE_ARG "")
endif()

execute_process(
    COMMAND ${DATE_COMMAND} ${DATE_ARG}
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE BUILD_DATE
    ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(
    COMMAND "${GIT_EXECUTABLE}" rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE GIT_BRANCH_NAME
    ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(
    COMMAND "${GIT_EXECUTABLE}" rev-parse --short HEAD
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

add_compile_definitions(BUILD_TYPE="${CMAKE_BUILD_TYPE}")
add_compile_definitions(BUILD_DATE="${BUILD_DATE}")
add_compile_definitions(GIT_BRANCH_NAME="${GIT_BRANCH_NAME}")
add_compile_definitions(GIT_COMMIT_HASH="${GIT_COMMIT_HASH}")

if(UNIX AND NOT APPLE)
    # Fixes QWebEngine linker errors on Ubuntu 20.04
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=gold")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=gold")
endif()
include(${CMAKE_CURRENT_SOURCE_DIR}/CMake/FetchContentThirdParty.cmake)

add_subdirectory(CMake)
add_subdirectory(ScreenPlay)
add_subdirectory(ScreenPlaySDK)
add_subdirectory(ScreenPlayShader)
add_subdirectory(ScreenPlayWallpaper)
add_subdirectory(ScreenPlayWidget)
add_subdirectory(ScreenPlayUtil)
add_subdirectory(ScreenPlayWeather)
add_subdirectory(ThirdParty/QArchive)
add_subdirectory(Tools)

if(${SCREENPLAY_TESTS})
    enable_testing()
endif()

# Only add target SteamSDKQtEnums
add_subdirectory(ScreenPlayWorkshop/SteamSDK)
if(${SCREENPLAY_STEAM})
    add_subdirectory(ScreenPlayWorkshop)
endif()

if(WIN32)
    add_subdirectory(ScreenPlaySysInfo)
endif()

if(${SCREENPLAY_INSTALLER})
    include(${CMAKE_CURRENT_SOURCE_DIR}/CMake/CreateIFWInstaller.cmake)
endif()

message(STATUS "[CPP DEFINE] DEPLOY_VERSION            = ${DEPLOY_VERSION}")
message(STATUS "[CPP DEFINE] SOURCE_DIR                = ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "[CPP DEFINE] BUILD_DATE                = ${BUILD_DATE}")
message(STATUS "[DEFINE] BUILD_TYPE                    = ${CMAKE_BUILD_TYPE}")
message(STATUS "[DEFINE] GIT_COMMIT_HASH               = ${GIT_COMMIT_HASH}")
message(STATUS "[OPTION] SCREENPLAY_DEPLOY             = ${SCREENPLAY_DEPLOY}")
message(STATUS "[OPTION] SCREENPLAY_INSTALLER          = ${SCREENPLAY_INSTALLER}")
message(STATUS "[OPTION] SCREENPLAY_STEAM              = ${SCREENPLAY_STEAM}")
message(STATUS "[OPTION] SCREENPLAY_TESTS              = ${SCREENPLAY_TESTS}")
message(STATUS "[PROJECT] SCREENPLAY_QML_MODULES_PATH  = ${SCREENPLAY_QML_MODULES_PATH}")
message(STATUS "[PROJECT] SCREENPLAY_IFW_VERSION       = ${SCREENPLAY_IFW_VERSION}")
message(STATUS "[PROJECT] CMAKE_TOOLCHAIN_FILE         = ${CMAKE_TOOLCHAIN_FILE}")
message(STATUS "[PROJECT] VCPKG_PATH                   = ${VCPKG_PATH}")
message(STATUS "[PROJECT] VCPKG_TARGET_TRIPLET         = ${VCPKG_TARGET_TRIPLET}")
message(STATUS "[PROJECT] CMAKE_PREFIX_PATH            = ${CMAKE_PREFIX_PATH}")
message(STATUS "[PROJECT] CMAKE_VERSION                = ${CMAKE_VERSION}")
