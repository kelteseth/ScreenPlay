cmake_minimum_required(VERSION 3.16.0)

project(
    ScreenPlay
    VERSION 0.15.0
    DESCRIPTION "Modern, Cross Plattform, Live Wallpaper, Widgets and AppDrawer!"
    HOMEPAGE_URL "https://screen-play.app/"
    LANGUAGES CXX)

if(WIN32)
    set(VCPKG_ARCH "x64-windows")
elseif(UNIX AND NOT APPLE)
    set(VCPKG_ARCH "x64-linux")
elseif(APPLE)
    set(VCPKG_ARCH "x64-osx")
endif()

# This sets cmake to compile all dlls into the main directory
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})

set(SCREENPLAY_IFW_ROOT "")
set(SCREENPLAY_IFW_VERSION "4.3")
set(SCREENPLAY_QML_MODULES_PATH "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/qml")
set(VCPKG_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../ScreenPlay-vcpkg")
set(VCPKG_INSTALLED_PATH "${VCPKG_PATH}/installed/${VCPKG_ARCH}")

if(UNIX AND NOT APPLE)
    # Fixes QWebEngine linker errors on Ubuntu 20.04
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=gold")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=gold")
endif()

option(SCREENPLAY_STEAM "For FOSS distribution so we do not bundle proprietary code." ON)
option(SCREENPLAY_TESTS "Enables UI tests." ON)
option(SCREENPLAY_INSTALLER "Indicates whether an installer via the Qt Installer Framework is created." OFF)

# Gitlab CI has many ENV variables. We use this one to check if the current build happens inside the CI
if(DEFINED ENV{CI_COMMIT_MESSAGE})
    set(GITLAB_CI true)
endif()

file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})


add_compile_definitions(COMPILE_INFO="Build Date: ${BUILD_DATE}. Git Hash: ${GIT_COMMIT_HASH}. ")
add_compile_definitions(SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

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

add_subdirectory(ScreenPlay)
add_subdirectory(ScreenPlaySDK)
add_subdirectory(ScreenPlayShader)
add_subdirectory(ScreenPlayWallpaper)
add_subdirectory(ScreenPlayWidget)
add_subdirectory(ScreenPlayUtil)
add_subdirectory(ScreenPlayWeather)
add_subdirectory(CMake)
add_subdirectory(Tools)

if(${SCREENPLAY_TESTS})
    enable_testing()
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/CMake/FetchContentThirdParty.cmake)

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

list(APPEND QML_DIRS "${SCREENPLAY_QML_MODULES_PATH}")
set(QML_IMPORT_PATH "${QML_DIRS}" CACHE STRING "Qt Creator extra qml import paths")

message(STATUS "[DEFINE] SOURCE_DIR                    = ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "[DEFINE] BUILD_DATE                    = ${BUILD_DATE}")
message(STATUS "[DEFINE] BUILD_TYPE                    = ${CMAKE_BUILD_TYPE}")
message(STATUS "[DEFINE] GIT_COMMIT_HASH               = ${GIT_COMMIT_HASH}")
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
