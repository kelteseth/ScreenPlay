cmake_minimum_required(VERSION 3.22.0)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})
include(GetProjectVersion)
get_project_version(PROJECT_VERSION)

# This must be set before project()
if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES
        "arm64;x86_64"
        CACHE STRING "" FORCE)
    set(SCREENPLAY_QML_MODULES_PATH "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ScreenPlay.app/Contents/MacOS/qml")
endif()

project(
    ScreenPlay
    VERSION ${PROJECT_VERSION}
    DESCRIPTION "Modern, Cross Plattform, Live Wallpaper, Widgets and AppDrawer!"
    HOMEPAGE_URL "https://screen-play.app/"
    LANGUAGES CXX)

# Only used for some custom file copy
if(WIN32)
    set(VCPKG_ARCH "x64-windows")
elseif(UNIX AND NOT APPLE)
    set(VCPKG_ARCH "x64-linux")
elseif(APPLE)
    set(VCPKG_ARCH "64-osx-universal") # Our own triplet for universal binaries
    set(VCPKG_TARGET_ARCHITECTURE "arm64;x86_64")
endif()

# This sets cmake to compile all dlls into the main directory
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_STANDARD 20)
set(THIRD_PARTY_PATH "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty")
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})
set(CMAKE_OSX_DEPLOYMENT_TARGET "13.0")

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

set(VCPKG_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../vcpkg")
set(VCPKG_INSTALLED_PATH "${VCPKG_PATH}/installed/${VCPKG_ARCH}")
set(VCPKG_BIN_PATH "${VCPKG_INSTALLED_PATH}/bin")

# Godot Editor
set(GODOT_VERSION "v4.2")
set(GODOT_RELEASE_TYPE "beta4")

if(WIN32)
    set(GODOT_EDITOR_NAME "Godot_${GODOT_VERSION}-${GODOT_RELEASE_TYPE}_win64.exe")
elseif(APPLE)
    set(GODOT_EDITOR_NAME "Godot.app")
elseif(UNIX)
    set(GODOT_EDITOR_NAME "Godot_${GODOT_VERSION}-${GODOT_RELEASE_TYPE}_linux.x86_64")
else()
    message(FATAL_ERROR "Unsupported OS")
endif()

option(SCREENPLAY_OSX_BUNDLE "Enable distribution macOS bundle" OFF)
option(SCREENPLAY_STEAM "For FOSS distribution so we do not bundle proprietary code." ON)
option(SCREENPLAY_DEPLOY "Marks this version as an official deploy version. This version uses different import paths and other settings."
       OFF)
option(SCREENPLAY_TESTS "Enables UI tests." ON)
option(SCREENPLAY_INSTALLER "Indicates whether an installer via the Qt Installer Framework is created." OFF)
option(SCREENPLAY_GODOT "Compiles ScreenPlayGodotWallpaper." ON)

# Gitlab CI has many ENV variables. We use this one to check if the current build happens inside the CI
if(DEFINED ENV{CI_COMMIT_MESSAGE})
    set(GITLAB_CI true)
endif()

file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

find_package(Git REQUIRED)

if(WIN32)
    set(DATE_COMMAND "CMD")
    set(DATE_ARG "/c date /t")
else()
    set(DATE_COMMAND "date")
    set(DATE_ARG "")
endif()

if(UNIX AND NOT APPLE)
    # Fixes QWebEngine linker errors on Ubuntu 22.04
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=gold")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=gold")
endif()

execute_process(
    COMMAND ${DATE_COMMAND} ${DATE_ARG}
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE BUILD_DATE
    OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(
    COMMAND "${GIT_EXECUTABLE}" rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE GIT_BRANCH_NAME
    OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(
    COMMAND "${GIT_EXECUTABLE}" rev-parse --short HEAD
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE)

if(${SCREENPLAY_TESTS})
    enable_testing()
endif()

if(UNIX AND NOT APPLE)
    # Needs to be append, because we include ecm as third party on linux
    set(ECM_DIR "${THIRD_PARTY_PATH}/ecm")
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")
    list(APPEND CMAKE_MODULE_PATH "${ECM_DIR}")
    list(APPEND CMAKE_MODULE_PATH "${ECM_DIR}/cmake")
endif()

add_subdirectory(ThirdParty)
add_subdirectory(CMake)
add_subdirectory(Tools)
add_subdirectory(ScreenPlay)
add_subdirectory(ScreenPlaySDK)
add_subdirectory(ScreenPlayShader)
add_subdirectory(ScreenPlayWallpaper)
add_subdirectory(ScreenPlayWidget)
add_subdirectory(ScreenPlayUtil)
add_subdirectory(ScreenPlayWeather)

if(${SCREENPLAY_GODOT})
    add_subdirectory(ScreenPlayWallpaper/Godot)
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
    if(APPLE)
        include(${CMAKE_CURRENT_SOURCE_DIR}/CMake/CreateDmgInstaller.cmake)
    else()
        include(${CMAKE_CURRENT_SOURCE_DIR}/CMake/CreateIFWInstaller.cmake)
    endif()
endif()

message(STATUS "[PROJECT] CMAKE_MODULE_PATH            = ${CMAKE_MODULE_PATH}")
message(STATUS "[PROJECT] PROJECT_VERSION              = ${PROJECT_VERSION}")
message(STATUS "[PROJECT] CMAKE_VERSION                = ${CMAKE_VERSION}")
message(STATUS "[PROJECT] SCREENPLAY_QML_MODULES_PATH  = ${SCREENPLAY_QML_MODULES_PATH}")
message(STATUS "[PROJECT] CMAKE_TOOLCHAIN_FILE         = ${CMAKE_TOOLCHAIN_FILE}")
message(STATUS "[PROJECT] VCPKG_PATH                   = ${VCPKG_PATH}")
message(STATUS "[PROJECT] CMAKE_MODULE_PATH            = ${CMAKE_MODULE_PATH}")
message(STATUS "[PROJECT] VCPKG_TARGET_TRIPLET         = ${VCPKG_TARGET_TRIPLET}")
message(STATUS "[PROJECT] CMAKE_PREFIX_PATH            = ${CMAKE_PREFIX_PATH}")
message(STATUS "[PROJECT] GODOT_VERSION                = ${GODOT_VERSION}")
message(STATUS "[PROJECT] GODOT_EDITOR_NAME            = ${GODOT_EDITOR_NAME}")
message(STATUS "[OPTION] SCREENPLAY_DEPLOY             = ${SCREENPLAY_DEPLOY}")
message(STATUS "[OPTION] SCREENPLAY_INSTALLER          = ${SCREENPLAY_INSTALLER}")
message(STATUS "[OPTION] SCREENPLAY_STEAM              = ${SCREENPLAY_STEAM}")
message(STATUS "[OPTION] SCREENPLAY_TESTS              = ${SCREENPLAY_TESTS}")
message(STATUS "[OPTION] SCREENPLAY_GODOT              = ${SCREENPLAY_GODOT}")
message(STATUS "[DEFINE] BUILD_TYPE                    = ${CMAKE_BUILD_TYPE}")
message(STATUS "[DEFINE] GIT_COMMIT_HASH               = ${GIT_COMMIT_HASH}")
message(STATUS "[CPP DEFINE] DEPLOY_VERSION            = ${DEPLOY_VERSION}")
message(STATUS "[CPP DEFINE] SOURCE_DIR                = ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "[CPP DEFINE] BUILD_DATE                = ${BUILD_DATE}")
