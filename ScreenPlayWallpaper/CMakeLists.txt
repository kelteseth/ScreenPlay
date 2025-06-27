# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
project(ScreenPlayWallpaper LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

find_package(
    Qt6
    COMPONENTS Core
               Quick
               Gui
               Widgets
               WebSockets
               Svg
               Multimedia
               WebEngineCore
               WebEngineQuick
               WebChannel
               Positioning)

qt_standard_project_setup(REQUIRES 6.8)

if(WIN32)
    set(SOURCES src/windowsdesktopproperties.cpp src/winwindow.cpp)
    set(HEADER src/windowsdesktopproperties.h src/winwindow.h)
elseif(APPLE)
    set(SOURCES src/macintegration.cpp src/macwindow.cpp)
    set(HEADER
        src/macbridge.h
        src/MacBridge.mm
        src/macintegration.h
        src/macwindow.h)
elseif(UNIX)
    set(SOURCES src/linuxx11window.cpp src/linuxwaylandwindow.cpp)
    set(HEADER src/linuxx11window.h src/linuxwaylandwindow.h)
endif()

set(SOURCES ${SOURCES} src/basewindow.cpp)
set(HEADER ${HEADER} src/basewindow.h)

set(QML
    # cmake-format: sort
    qml/CrossFadeVideoPlayer.qml
    qml/GifWallpaper.qml
    qml/MultimediaView.qml
    qml/MultimediaWebView.qml
    qml/Test.qml
    qml/WebsiteWallpaper.qml
    ScreenPlayWallpaperMain.qml)

set(RESOURCES qtquickcontrols2.conf index.html)

# Create a separate Windows Integration library
if(WIN32)
    qt_add_library(ScreenPlayWallpaperWindowsIntegration STATIC)
    target_sources(
        ScreenPlayWallpaperWindowsIntegration
        PRIVATE src/windowsintegration.cpp
        PUBLIC src/windowsintegration.h)
    target_compile_features(ScreenPlayWallpaperWindowsIntegration PRIVATE cxx_std_23)
    target_include_directories(ScreenPlayWallpaperWindowsIntegration PUBLIC src)

    # Used for query windows monitor data
    target_link_libraries(ScreenPlayWallpaperWindowsIntegration PUBLIC ScreenPlayCoreWindowsExtrasLib shcore.lib)
endif()

qt_add_library(ScreenPlayWallpaperLib STATIC)
target_compile_features(ScreenPlayWallpaperLib PRIVATE cxx_std_23)

# qml_typeregistration
target_include_directories(ScreenPlayWallpaperLib PUBLIC src)

qt_add_qml_module(
    ScreenPlayWallpaperLib
    URI
    ScreenPlayWallpaper
    VERSION
    1.0
    QML_FILES
    ${QML}
    SOURCES
    ${SOURCES}
    ${HEADER}
    RESOURCES
    ${RESOURCES})

target_link_libraries(
    ScreenPlayWallpaperLib
    PUBLIC ScreenPlaySDKLib
           ScreenPlayCoreLibplugin
           ScreenPlayWeatherLibplugin
           Qt6::Quick
           Qt6::Gui
           Qt6::Widgets
           Qt6::Core
           Qt6::WebSockets
           Qt6::Svg
           Qt6::Multimedia
           Qt6::WebEngineCore
           Qt6::WebEngineQuick)

if(WIN32)
    target_link_libraries(ScreenPlayWallpaperLib PUBLIC ScreenPlayWallpaperWindowsIntegration ScreenPlaySysInfoLibplugin
                                                        ScreenPlayCoreWindowsExtrasLib)
elseif(UNIX AND NOT APPLE)
    target_link_libraries(ScreenPlayWallpaperLib PRIVATE X11)
endif()

qt_add_executable(SPWallpaper main.cpp)
target_compile_features(SPWallpaper PRIVATE cxx_std_23)
target_link_libraries(SPWallpaper PRIVATE ScreenPlayWallpaperLib ScreenPlayWallpaperLibplugin)

if(UNIX AND NOT APPLE)
    find_package(
        ECM
        CONFIG
        REQUIRED
        NO_MODULE)
    set(LayerShellQt "${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/layer-shell-qt/")
    find_package(LayerShellQt REQUIRED)
    target_link_libraries(ScreenPlayWallpaperLib PRIVATE LayerShellQtInterface)
endif()

if(APPLE)
    target_link_libraries(ScreenPlayWallpaperLib PRIVATE "-framework Cocoa")

    add_custom_command(
        TARGET SPWallpaper
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/index.html"
                "$<TARGET_BUNDLE_DIR:ScreenPlayApp>/Contents/MacOS/")
endif()

if(WIN32)
    # Disable console window on Windows
    # https://stackoverflow.com/questions/8249028/how-do-i-keep-my-qt-c-program-from-opening-a-console-in-windows
    set_property(TARGET SPWallpaper PROPERTY WIN32_EXECUTABLE true)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/index.html ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/index.html COPYONLY)
endif()

# ##### USE CMAKE VARIABLES IN CODE #####
include(GenerateCMakeVariableHeader)
generate_cmake_variable_header(SPWallpaper)

include(GNUInstallDirs)
install(TARGETS SPWallpaper BUNDLE DESTINATION .)

register_qt_additional_deployment(SPWallpaper)
