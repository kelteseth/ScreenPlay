
project(ScreenPlayWidget LANGUAGES CXX)

set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

find_package(
    Qt5
    COMPONENTS Quick Widgets Gui QuickCompiler WebEngine
    REQUIRED)

set(src main.cpp src/widgetwindow.cpp)

set(headers src/widgetwindow.h)

if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(resources SPWidgetResources.qrc)
else()
    qtquick_compiler_add_resources(resources SPWidgetResources.qrc)
endif()

add_executable(${PROJECT_NAME} ${src} ${headers} ${resources})

# Disable console window on Windows
# https://stackoverflow.com/questions/8249028/how-do-i-keep-my-qt-c-program-from-opening-a-console-in-windows
set_property(TARGET ${PROJECT_NAME} PROPERTY WIN32_EXECUTABLE true)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Quick Qt5::Gui Qt5::Widgets Qt5::Core ScreenPlaySDK)
