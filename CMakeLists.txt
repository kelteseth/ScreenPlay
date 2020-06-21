cmake_minimum_required(VERSION 3.16.0 )

# This sets cmake to compile all dlls into the main directory
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_STANDARD 20)


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



