cmake_minimum_required(VERSION 3.16.0 )

# This sets cmake to compile all dlls into the main directory
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_STANDARD 20)

if(WIN32)
    set(VCPKG_ARCH "x64-windows")
elseif(UNIX)
    set(VCPKG_ARCH "x64-linux")
elseif(APPLE)
    set(VCPKG_ARCH "x64-osx")
endif()

set(VCPKG_INCLUDE_PATH  ${CMAKE_CURRENT_SOURCE_DIR}/Common/vcpkg/installed/${VCPKG_ARCH}/include/)
set(QT_TELEMTRY_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/Common/qt-google-analytics/)
set(QT_BREAKPAD_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/Common/qt-breakpad/)


find_package(Git REQUIRED)
if(WIN32)
    set(date_command "CMD" )
    set(date_arg "/c date /t")

elseif(APPLE)
    set(date_command "date")
    set(date_arg " +%y.%m.%d")
elseif(UNIX)
    set(date_command "date")
    set(date_arg "")
endif()

execute_process(COMMAND
  ${date_command}  ${date_arg}
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  OUTPUT_VARIABLE BUILD_DATE
  ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(COMMAND
  "${GIT_EXECUTABLE}" describe --always
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  OUTPUT_VARIABLE GIT_COMMIT_HASH
  ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

add_compile_definitions(COMPILE_INFO="${BUILD_DATE} + ${GIT_COMMIT_HASH}")

project(ScreenPlay)

add_subdirectory(ScreenPlay)
add_subdirectory(ScreenPlaySDK)
add_subdirectory(ScreenPlayWallpaper)
add_subdirectory(ScreenPlayWidget)
add_subdirectory(ScreenPlaySysInfo)
add_subdirectory(Common/stomt-qml)
add_subdirectory(Common/qt-breakpad)



