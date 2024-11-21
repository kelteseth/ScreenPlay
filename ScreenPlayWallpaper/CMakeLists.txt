# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
project(ScreenPlayWallpaper LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
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

set(SOURCES ${SOURCES} main.cpp src/basewindow.cpp)
set(HEADER ${HEADER} src/basewindow.h)

set(QML
    # cmake-format: sort
    qml/GifWallpaper.qml
    qml/MultimediaView.qml
    qml/MultimediaWebView.qml
    qml/Test.qml
    qml/Wallpaper.qml
    qml/WebsiteWallpaper.qml)

set(RESOURCES qtquickcontrols2.conf index.html)

set(LIB_SOURCES)
set(LIB_HEADER)

if(WIN32)
    set(LIB_SOURCES src/windowsintegration.cpp)
    set(LIB_HEADER src/windowsintegration.h)
    add_library(ScreenPlayWallpaperLib STATIC ${LIB_SOURCES} ${LIB_HEADER})
    target_include_directories(ScreenPlayWallpaperLib PUBLIC src)
    # Used for query windows monitor data
    target_link_libraries(ScreenPlayWallpaperLib PUBLIC shcore.lib)
endif()

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADER})

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

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE ScreenPlaySDK
            ScreenPlayCoreplugin
            ScreenPlayWeatherplugin
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
    target_link_libraries(${PROJECT_NAME} PRIVATE ScreenPlaySysInfoplugin ScreenPlayWallpaperLib)
elseif(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE X11)
endif()

if(UNIX AND NOT APPLE)
    find_package(
        ECM
        CONFIG
        REQUIRED
        NO_MODULE)
    set(LayerShellQt "${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/layer-shell-qt/")
    find_package(LayerShellQt REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE LayerShellQtInterface)
endif()

if(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework Cocoa")

    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/index.html
                ${CMAKE_BINARY_DIR}/bin/ScreenPlay.app/Contents/MacOS/)
    set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/ScreenPlay.app/Contents/MacOS/")

endif()

if(WIN32)
    # Disable console window on Windows
    # https://stackoverflow.com/questions/8249028/how-do-i-keep-my-qt-c-program-from-opening-a-console-in-windows
    set_property(TARGET ${PROJECT_NAME} PROPERTY WIN32_EXECUTABLE true)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/index.html ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/index.html COPYONLY)
endif()

# ##### USE CMAKE VARIABLES IN CODE #####
include(GenerateCMakeVariableHeader)
generate_cmake_variable_header(${PROJECT_NAME})
