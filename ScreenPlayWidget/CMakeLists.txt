project(ScreenPlayWidget LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core)
set(src main.cpp src/widgetwindow.cpp)

set(headers src/widgetwindow.h)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    find_package(
        Qt${QT_VERSION_MAJOR}
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

    if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        qt_add_resources(resources SPWidgetResources.qrc)
    else()
        qtquick_compiler_add_resources(resources SPWidgetResources.qrc)
    endif()

    add_executable(${PROJECT_NAME} ${src} ${headers} ${resources})

    target_link_libraries(
        ${PROJECT_NAME}
        PRIVATE Qt${QT_VERSION_MAJOR}::Quick
                Qt${QT_VERSION_MAJOR}::Gui
                Qt${QT_VERSION_MAJOR}::Widgets
                Qt${QT_VERSION_MAJOR}::Core
                Qt${QT_VERSION_MAJOR}::WebSockets
                Qt${QT_VERSION_MAJOR}::Svg
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

    if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        qt_add_resources(resources SPWidgetResources.qrc)
    else()
        qtquick_compiler_add_resources(resources SPWidgetResources.qrc)
    endif()

    add_executable(${PROJECT_NAME} ${src} ${headers} ${resources})

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

target_link_libraries(${PROJECT_NAME} PRIVATE ScreenPlaySDK ScreenPlayUtil)

if(WIN32)
    # Disable console window on Windows
    # https://stackoverflow.com/questions/8249028/how-do-i-keep-my-qt-c-program-from-opening-a-console-in-windows
    set_property(TARGET ${PROJECT_NAME} PROPERTY WIN32_EXECUTABLE true)
endif()

if(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE TRUE MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist)
endif()
