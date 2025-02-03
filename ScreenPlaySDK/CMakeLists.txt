# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
project(ScreenPlaySDK LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

find_package(
    Qt6
    COMPONENTS Quick Network Core
    REQUIRED)

qt_standard_project_setup(REQUIRES 6.8)

set(SOURCES src/screenplaysdk.cpp)

set(HEADER inc/public/ScreenPlaySDK/screenplaysdk.h)

qt_add_library(ScreenPlaySDKLib STATIC ${SOURCES} ${HEADER})
target_compile_features(ScreenPlaySDKLib PRIVATE cxx_std_23)

target_include_directories(ScreenPlaySDKLib PUBLIC inc/public/)

target_link_libraries(
    ScreenPlaySDKLib
    PUBLIC 
    Qt6::Core
    Qt6::Quick
    Qt6::Gui
    Qt6::Network
    ScreenPlayCoreLib)
