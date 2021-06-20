project(ScreenPlayWidget LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)
find_package(
    QT NAMES Qt6 Qt5
    COMPONENTS Quick Widgets Gui
    REQUIRED)
find_package(
    Qt${QT_VERSION_MAJOR}
    COMPONENTS Quick Widgets Gui WebEngine
    REQUIRED)

set(src main.cpp src/widgetwindow.cpp)

set(headers src/widgetwindow.h)

if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(resources SPWidgetResources.qrc)
else()
    qtquick_compiler_add_resources(resources SPWidgetResources.qrc)
endif()

add_executable(${PROJECT_NAME} ${src} ${headers} ${resources})

if(WIN32)
    # Disable console window on Windows
    # https://stackoverflow.com/questions/8249028/how-do-i-keep-my-qt-c-program-from-opening-a-console-in-windows
    set_property(TARGET ${PROJECT_NAME} PROPERTY WIN32_EXECUTABLE true)
endif()

if(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE true)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Quick Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Widgets
                                              Qt${QT_VERSION_MAJOR}::Core ScreenPlaySDK ScreenPlayUtil)
