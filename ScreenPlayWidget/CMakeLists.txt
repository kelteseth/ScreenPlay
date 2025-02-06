# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
project(ScreenPlayWidget LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

set(SOURCES  src/widgetwindow.cpp)

set(HEADER src/widgetwindow.h)

set(QML qml/Test.qml ScreenPlayWidgetMain.qml)

set(RESOURCES
    # cmake-format: sort
    assets/icons/baseline-close-24px.svg
    assets/icons/baseline-opacity-24px.svg
    assets/icons/baseline-settings-20px.svg
    assets/image/noisy-texture-3.png)


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

qt_standard_project_setup(REQUIRES 6.8)

qt_add_library(ScreenPlayWidgetLib STATIC)
# qml_typeregistration
target_include_directories(ScreenPlayWidgetLib PUBLIC src)
qt_add_executable(SPWidget main.cpp)

target_link_libraries(
    ScreenPlayWidgetLib
    PUBLIC ScreenPlaySDKLib
            ScreenPlayCoreLib
            ScreenPlayCoreLibplugin
            ScreenPlayWeatherLibplugin
            Qt6::Quick
            Qt6::Gui
            Qt6::Widgets
            Qt6::Core
            Qt6::WebSockets
            Qt6::Svg
            Qt6::WebEngineCore
            Qt6::WebEngineQuick)

qt_add_qml_module(
    ScreenPlayWidgetLib
    URI
    ScreenPlayWidget
    VERSION
    1.0
    QML_FILES
    ${QML}
    SOURCES
    ${SOURCES} 
    ${HEADER}
    RESOURCES
    ${RESOURCES})

if(WIN32)
    target_link_libraries(SPWidget PRIVATE ScreenPlaySysInfoLibplugin)
endif()

target_link_libraries(SPWidget PRIVATE ScreenPlayWidgetLib ScreenPlayWidgetLibplugin)

if(WIN32)
    # Disable console window on Windows
    # https://stackoverflow.com/questions/8249028/how-do-i-keep-my-qt-c-program-from-opening-a-console-in-windows
    set_property(TARGET SPWidget PROPERTY WIN32_EXECUTABLE true)
endif()

# ##### USE CMAKE VARIABLES IN CODE #####
include(GenerateCMakeVariableHeader)
generate_cmake_variable_header(SPWidget)


include(GNUInstallDirs)
install(TARGETS SPWidget
    BUNDLE DESTINATION .
)

register_qt_additional_deployment(SPWidget)