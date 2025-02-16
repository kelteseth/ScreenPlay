cmake_minimum_required(VERSION 3.22.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(THIRD_PARTY_PATH "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty")
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})

include(GetProjectVersion)
get_project_version(SCREENPLAY_VERSION)

# This must be set before project()
if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES
        "arm64;x86_64"
        CACHE STRING "" FORCE)
endif()

project(
    ScreenPlay
    VERSION ${SCREENPLAY_VERSION}
    DESCRIPTION "Modern, Cross Plattform, Live Wallpaper, Widgets and AppDrawer!"
    HOMEPAGE_URL "https://screen-play.app/"
    LANGUAGES CXX)

find_package(
    Qt6
    COMPONENTS Core
               Quick
               QuickControls2
               Gui
               Widgets
               WebSockets
               Svg
               Xml
               LinguistTools
               Test)

include(QtDeploy)

# Only used for some custom file copy
if(WIN32)
    set(VCPKG_TARGET_TRIPLET "x64-windows")
elseif(UNIX AND NOT APPLE)
    set(VCPKG_TARGET_TRIPLET "x64-linux")
elseif(APPLE)
    set(VCPKG_TARGET_TRIPLET "64-osx-universal") # Our own triplet for universal binaries
    set(VCPKG_TARGET_ARCHITECTURE "arm64;x86_64")
endif()

# QArchive: RPATH is the runtime library search path on Unix systems that tells the dynamic linker where to look for shared libraries. By
# default, CMake builds with RPATH pointing to the build directory, then changes it during installation. Some build systems like Ninja on
# non-ELF platforms don't support this RPATH modification during install. Setting CMAKE_BUILD_WITH_INSTALL_RPATH builds directly with the
# final installation RPATH, avoiding the need to modify it later.
if(UNIX)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
endif()

set(VCPKG_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../vcpkg")
set(VCPKG_INSTALLED_PATH "${VCPKG_PATH}/installed/${VCPKG_TARGET_TRIPLET}")
set(VCPKG_BIN_PATH "${VCPKG_INSTALLED_PATH}/bin")

# Godot Editor
set(GODOT_VERSION_MAJOR "4")
set(GODOT_VERSION_MINOR "3")
set(GODOT_VERSION_PATCH "")
set(GODOT_RELEASE_TYPE "stable")

# Use an if statement to check if GODOT_VERSION_PATCH is empty or not
if(GODOT_VERSION_PATCH STREQUAL "")
    # If patch version is empty, don't include it and the preceding dot
    set(GODOT_VERSION "v${GODOT_VERSION_MAJOR}.${GODOT_VERSION_MINOR}-${GODOT_RELEASE_TYPE}")
else()
    # If patch version is not empty, include it and the preceding dot
    set(GODOT_VERSION "v${GODOT_VERSION_MAJOR}.${GODOT_VERSION_MINOR}.${GODOT_VERSION_PATCH}-${GODOT_RELEASE_TYPE}")
endif()

if(WIN32)
    set(GODOT_EDITOR_NAME "Godot_${GODOT_VERSION}_win64.exe")
elseif(APPLE)
    set(GODOT_EDITOR_NAME "Godot.app")
elseif(UNIX)
    set(GODOT_EDITOR_NAME "Godot_${GODOT_VERSION}_linux.x86_64")
else()
    message(FATAL_ERROR "Unsupported OS")
endif()

option(QCORO_BUILD_TESTING "Build QCoro tests" OFF)
option(QCORO_BUILD_EXAMPLES "Build QCoro examples" OFF)
option(SCREENPLAY_STEAM "For FOSS distribution so we do not bundle proprietary code." ON)
option(SCREENPLAY_DEPLOY "Marks this version as an official deploy version. This version uses different import paths and other settings."
       OFF)
option(SCREENPLAY_TESTS "Enables UI tests." OFF)
option(SCREENPLAY_INSTALLER "Indicates whether an installer via the Qt Installer Framework is created." OFF)
option(SCREENPLAY_GODOT "Compiles ScreenPlayGodotWallpaper." ON)

if(${SCREENPLAY_STEAM})
    if(WIN32)
        set(STEAM_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/Steam/win64")
        set(STEAM_BINARY "steam_api64.dll")
    elseif(APPLE)
        set(STEAM_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/Steam/osx")
        set(STEAM_BINARY "libsteam_api.dylib")
    elseif(UNIX)
        set(STEAM_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/Steam/linux64")
        set(STEAM_BINARY "libsteam_api.so")
    endif()

    set(STEAM_BINARY_PATH "${STEAM_ROOT}/${STEAM_BINARY}")
endif()
# Gitlab CI has many ENV variables. We use this one to check if the current build happens inside the CI
if(DEFINED ENV{CI_COMMIT_MESSAGE})
    set(GITLAB_CI true)
endif()

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
add_subdirectory(ScreenPlayCore)
add_subdirectory(ScreenPlayWeather)
# Only add target SteamSDKQtEnums
add_subdirectory(ScreenPlayWorkshop/SteamSDK)

if(${SCREENPLAY_GODOT})
    add_subdirectory(ScreenPlayWallpaper/Godot)
endif()

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

# Generate single deployment script after all targets are defined
generate_qt_deployment_script()

message(STATUS "[PROJECT] CMAKE_MODULE_PATH            = ${CMAKE_MODULE_PATH}")
message(STATUS "[PROJECT] CMAKE_VERSION                = ${CMAKE_VERSION}")
message(STATUS "[PROJECT] SCREENPLAY_QML_MODULES_PATH  = ${SCREENPLAY_QML_MODULES_PATH}")
message(STATUS "[PROJECT] CMAKE_TOOLCHAIN_FILE         = ${CMAKE_TOOLCHAIN_FILE}")
message(STATUS "[PROJECT] VCPKG_PATH                   = ${VCPKG_PATH}")
message(STATUS "[PROJECT] CMAKE_MODULE_PATH            = ${CMAKE_MODULE_PATH}")
message(STATUS "[PROJECT] VCPKG_TARGET_TRIPLET         = ${VCPKG_TARGET_TRIPLET}")
message(STATUS "[PROJECT] QT_QML_GENERATE_QMLLS_INI    = ${QT_QML_GENERATE_QMLLS_INI}")
message(STATUS "[PROJECT] QT_QML_DEBUG                 = ${QT_QML_DEBUG}")
message(STATUS "[PROJECT] CMAKE_PREFIX_PATH            = ${CMAKE_PREFIX_PATH}")
message(STATUS "[PROJECT] GODOT_VERSION                = ${GODOT_VERSION}")
message(STATUS "[PROJECT] GODOT_EDITOR_NAME            = ${GODOT_EDITOR_NAME}")
message(STATUS "[PROJECT] SCREENPLAY_VERSION           = ${SCREENPLAY_VERSION}")
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
