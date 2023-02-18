# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
project(ScreenPlayWidget LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

set(SOURCES main.cpp src/widgetwindow.cpp)

set(HEADER src/widgetwindow.h)

set(QML qml/Test.qml qml/Widget.qml)

find_package(
    Qt6
    COMPONENTS Core
               Quick
               Gui
               Widgets
               WebSockets
               Svg
               WebEngineCore
               WebEngineQuick
               WebChannel
               Positioning)

set(RESOURCES
    # cmake-format: sort
    assets/icons/baseline-close-24px.svg
    assets/icons/baseline-opacity-24px.svg
    assets/icons/baseline-settings-20px.svg
    assets/image/noisy-texture-3.png)

qt_add_executable(
    ${PROJECT_NAME}
    ${SOURCES}
    ${HEADER}
    ${RESOURCES})

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE ScreenPlaySDK
            ScreenPlayUtil
            ScreenPlayWeatherplugin
            Qt6::Quick
            Qt6::Gui
            Qt6::Widgets
            Qt6::Core
            Qt6::WebSockets
            Qt6::Svg
            Qt6::WebEngineCore
            Qt6::WebEngineQuick)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE ScreenPlaySysInfoplugin)
endif()

qt_add_qml_module(
    ${PROJECT_NAME}
    URI
    ${PROJECT_NAME}
    VERSION
    1.0
    OUTPUT_DIRECTORY
    ${SCREENPLAY_QML_MODULES_PATH}/${PROJECT_NAME}
    RESOURCE_PREFIX
    /qml
    QML_FILES
    ${QML}
    RESOURCES
    ${RESOURCES})

if(WIN32)
    # Disable console window on Windows
    # https://stackoverflow.com/questions/8249028/how-do-i-keep-my-qt-c-program-from-opening-a-console-in-windows
    set_property(TARGET ${PROJECT_NAME} PROPERTY WIN32_EXECUTABLE true)
endif()

if(APPLE AND NOT OSX_BUNDLE)
    set_target_properties(${PROJECT_NAME}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/ScreenPlay.app/Contents/MacOS/"
    )
endif()


# ##### USE CMAKE VARIABLES IN CODE #####
include(GenerateCmakeVariableHeader)
generate_cmake_variable_header(${PROJECT_NAME})