# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
project(ScreenPlayWidget LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

set(SOURCES src/widgetwindow.cpp)

set(HEADER src/widgetwindow.h)

set(QML qml/Test.qml ScreenPlayWidgetMain.qml)

set(RESOURCES
    # cmake-format: sort
    assets/icons/baseline-close-24px.svg
    assets/icons/baseline-opacity-24px.svg
    assets/icons/baseline-settings-20px.svg
    assets/image/noisy-texture-3.png
    qtquickcontrols2.conf)

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

# Make sentry win only for now because it is constantly buggy on osx
if(WIN32)
    find_package(sentry CONFIG REQUIRED)
endif()

qt_standard_project_setup(REQUIRES 6.8)

qt_add_library(ScreenPlayWidgetLib STATIC)

# qml_typeregistration
target_include_directories(ScreenPlayWidgetLib PUBLIC src)

target_link_libraries(
    ScreenPlayWidgetLib
    PUBLIC ScreenPlaySDKLib
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

qt_add_executable(SPWidget main.cpp)
target_compile_features(SPWidget PRIVATE cxx_std_23)
target_link_libraries(SPWidget PRIVATE ScreenPlayWidgetLib ScreenPlayWidgetLibplugin)

if(WIN32)
    target_link_libraries(ScreenPlayWidgetLib PUBLIC ScreenPlaySysInfoLibplugin sentry::sentry)
    target_compile_definitions(ScreenPlayWidgetLib PRIVATE SENTRY_BACKEND_CRASHPAD CLIENT_STACKTRACES_ENABLED)

    # Disable console window on Windows
    # https://stackoverflow.com/questions/8249028/how-do-i-keep-my-qt-c-program-from-opening-a-console-in-windows
    set_property(TARGET SPWidget PROPERTY WIN32_EXECUTABLE true)
    target_compile_definitions(SPWidget PRIVATE SENTRY_BACKEND_CRASHPAD CLIENT_STACKTRACES_ENABLED)

    configure_file(${VCPKG_INSTALLED_PATH}/tools/sentry-native/crashpad_handler.exe ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ COPYONLY)
    configure_file(${VCPKG_INSTALLED_PATH}/tools/sentry-native/zlib1.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ COPYONLY)

    configure_file(${VCPKG_BIN_PATH}/libssl-3-x64.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ COPYONLY)
    configure_file(${VCPKG_BIN_PATH}/libcrypto-3-x64.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ COPYONLY)
endif()

# ##### USE CMAKE VARIABLES IN CODE #####
include(GenerateCMakeVariableHeader)
generate_cmake_variable_header(SPWidget)

include(GNUInstallDirs)
install(TARGETS SPWidget BUNDLE DESTINATION .)

register_qt_additional_deployment(SPWidget)
