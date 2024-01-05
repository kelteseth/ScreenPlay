# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
project(ScreenPlaySDK LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

find_package(
    Qt6
    COMPONENTS Quick Network Core
    REQUIRED)

set(SOURCES src/screenplaysdk.cpp)

set(HEADER inc/public/ScreenPlaySDK/screenplaysdk.h)

add_library(${PROJECT_NAME} ${SOURCES} ${HEADER})

target_include_directories(${PROJECT_NAME} PUBLIC inc/public/)

target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC Qt6::Core
            Qt6::Quick
            Qt6::Gui
            Qt6::Network
            ScreenPlayUtil)
