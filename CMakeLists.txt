cmake_minimum_required(VERSION 3.16.0 )

if(WIN32)
  set(VCPKG_TARGET_TRIPLET "x64-windows")
elseif(APPLE)
  set(VCPKG_TARGET_TRIPLET "x64-osx")
elseif(UNIX)
  set(VCPKG_TARGET_TRIPLET "x64-linux")
endif()

set(ABSOLUTE_VCPKG_PATH ${CMAKE_CURRENT_SOURCE_DIR}/scripts/buildsystems/vcpkg.cmake)

# If setting the toolchain file fails we can the it inside QtCreator: Extras ->
# Tools -> Kits -> <Your Kit> -> CMake Configuration -> Append this:
# CMAKE_TOOLCHAIN_FILE:STRING=%{CurrentProject:Path}/Common/vcpkg/scripts/buildsystems/vcpkg.cmake
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE ${ABSOLUTE_VCPKG_PATH})
endif()

# This sets cmake to compile all dlls into the main directory
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_STANDARD 17)

set(QT_TELEMTRY_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/Common/qt-google-analytics/)
set(QT_BREAKPAD_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/Common/qt-breakpad/)

project(ScreenPlay)

add_subdirectory(ScreenPlay)
add_subdirectory(ScreenPlaySDK)
add_subdirectory(ScreenPlayWallpaper)
add_subdirectory(ScreenPlayWidget)
add_subdirectory(ScreenPlaySysInfo)
add_subdirectory(Common/stomt-qml)
add_subdirectory(Common/qt-breakpad)



