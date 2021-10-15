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
    set(SOURCES # cmake-format: sortable
                src/windowsdesktopproperties.cpp src/winwindow.cpp)
    set(HEADER # cmake-format: sortable
               src/windowsdesktopproperties.h src/winwindow.h)
elseif(APPLE)
    set(SOURCES # cmake-format: sortable
                src/macintegration.cpp src/macwindow.cpp)
    set(HEADER # cmake-format: sortable
               src/macintegration.h src/macbridge.h src/macwindow.h src/MacBridge.mm)
elseif(UNIX)
    set(SOURCES # cmake-format: sortable
                src/linuxwindow.cpp)
    set(HEADER # cmake-format: sortable
               src/linuxwindow.h)
endif()

set(SOURCES ${SOURCES} main.cpp src/basewindow.cpp)
set(HEADER ${HEADER} src/basewindow.h)

set(QML # cmake-format: sortable
        qml/GifWallpaper.qml qml/Test.qml qml/Wallpaper.qml qml/WebsiteWallpaper.qml qml/WebView.qml qml/MultimediaView.qml)

qt_add_resources(RESOURCES Resources.qrc)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADER} ${RESOURCES})

qt_add_qml_module(
    ${PROJECT_NAME}
    URI
    ${PROJECT_NAME}
    VERSION
    1.0
    QML_FILES
    ${QML})

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE ScreenPlaySDK
            ScreenPlayUtil
            Qt6::Quick
            Qt6::Gui
            Qt6::Widgets
            Qt6::Core
            Qt6::WebSockets
            Qt6::Svg
            Qt6::Multimedia
            Qt6::WebEngineCore
            Qt6::WebEngineQuick)

if(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE TRUE MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework Cocoa")
endif()

if(WIN32)
    # Disable console window on Windows
    # https://stackoverflow.com/questions/8249028/how-do-i-keep-my-qt-c-program-from-opening-a-console-in-windows
    set_property(TARGET ${PROJECT_NAME} PROPERTY WIN32_EXECUTABLE true)
    target_link_libraries(${PROJECT_NAME} PRIVATE shcore.lib)
endif()
