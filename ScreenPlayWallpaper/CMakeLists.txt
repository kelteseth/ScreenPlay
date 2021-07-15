project(ScreenPlayWallpaper LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core)

if(WIN32)
    set(src_plattform src/windowsdesktopproperties.cpp src/winwindow.cpp)
    set(headers_plattform src/windowsdesktopproperties.h src/winwindow.h)
elseif(APPLE)
    set(src_plattform src/macintegration.cpp src/macwindow.cpp)
    set(headers_plattform src/macintegration.h src/macbridge.h src/macwindow.h src/MacBridge.mm)
elseif(UNIX)
    set(src_plattform main.cpp src/linuxwindow.cpp)
    set(headers_plattform src/linuxwindow.h)
endif()

set(src main.cpp src/basewindow.cpp)
set(headers src/basewindow.h)

if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    qt6_add_resources (resources SPWResources.qrc)
else()
    qt6_add_resources (resources SPWResources.qrc)
endif()

add_executable(${PROJECT_NAME} ${src} ${headers} ${src_plattform} ${headers_plattform} ${resources})

if(WIN32)
    # Disable console window on Windows
    # https://stackoverflow.com/questions/8249028/how-do-i-keep-my-qt-c-program-from-opening-a-console-in-windows
    set_property(TARGET ${PROJECT_NAME} PROPERTY WIN32_EXECUTABLE true)
    target_link_libraries(${PROJECT_NAME} PRIVATE shcore.lib)
endif()

if(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE TRUE MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework Cocoa")
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE ScreenPlaySDK ScreenPlayUtil)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    find_package(
        Qt${QT_VERSION_MAJOR}
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
    target_link_libraries(
        ${PROJECT_NAME}
        PRIVATE Qt${QT_VERSION_MAJOR}::Quick
                Qt${QT_VERSION_MAJOR}::Gui
                Qt${QT_VERSION_MAJOR}::Widgets
                Qt${QT_VERSION_MAJOR}::Core
                Qt${QT_VERSION_MAJOR}::WebSockets
                Qt${QT_VERSION_MAJOR}::Svg
                Qt6::Multimedia
                Qt6::WebEngineCore
                Qt6::WebEngineQuick)
else()
    find_package(
        Qt${QT_VERSION_MAJOR}
        COMPONENTS Core
                   Quick
                   Gui
                   Widgets
                   WebSockets
                   Svg
                   WebEngine)
    target_link_libraries(
        ${PROJECT_NAME}
        PRIVATE Qt${QT_VERSION_MAJOR}::Quick
                Qt${QT_VERSION_MAJOR}::Gui
                Qt${QT_VERSION_MAJOR}::Widgets
                Qt${QT_VERSION_MAJOR}::Core
                Qt${QT_VERSION_MAJOR}::WebSockets
                Qt${QT_VERSION_MAJOR}::Svg
                Qt5::WebEngine)
endif()
