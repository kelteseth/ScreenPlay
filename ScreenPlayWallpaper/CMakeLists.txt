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
    set(SOURCES src/linuxwindow.cpp)
    set(HEADER src/linuxwindow.h)
endif()

set(SOURCES ${SOURCES} main.cpp src/basewindow.cpp)
set(HEADER ${HEADER} src/basewindow.h)

set(QML
    qml/GifWallpaper.qml
    qml/MultimediaView.qml
    qml/MultimediaWebView.qml
    qml/Test.qml
    qml/Wallpaper.qml
    qml/WebsiteWallpaper.qml)

qt_add_resources(RESOURCES Resources.qrc)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADER} ${RESOURCES})

qt_add_qml_module(
    ${PROJECT_NAME}
    URI
    ${PROJECT_NAME}
    VERSION
    1.0
    OUTPUT_DIRECTORY
    ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}
    QML_FILES
    ${QML})

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
            Qt6::Multimedia
            Qt6::WebEngineCore
            Qt6::WebEngineQuick)
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE ScreenPlaySysInfoplugin)
endif()

if(UNIX AND NOT APPLE)
    include(CopyRecursive)
    copy_recursive(${CMAKE_CURRENT_SOURCE_DIR}/kde/ScreenPlay ${CMAKE_BINARY_DIR}/bin/kde/ScreenPlay "*")
endif()

if(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE TRUE MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework Cocoa")
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}.app/Contents/MacOS/)
    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/index.html
                ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}.app/Contents/MacOS/)

endif()

if(WIN32)
    # Disable console window on Windows
    # https://stackoverflow.com/questions/8249028/how-do-i-keep-my-qt-c-program-from-opening-a-console-in-windows
    set_property(TARGET ${PROJECT_NAME} PROPERTY WIN32_EXECUTABLE true)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/index.html ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/index.html COPYONLY)
endif()
