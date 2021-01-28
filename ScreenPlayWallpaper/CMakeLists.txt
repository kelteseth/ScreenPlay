project(ScreenPlayWallpaper LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

find_package(
    Qt5
    COMPONENTS Quick Widgets Gui WebEngine QuickCompiler
    REQUIRED)

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
    set(resources SPWResources.qrc)
else()
    qtquick_compiler_add_resources(resources SPWResources.qrc)
endif()

add_executable(${PROJECT_NAME} ${src} ${headers} ${src_plattform} ${headers_plattform} ${resources})

if(WIN32)
    # Disable console window on Windows
    # https://stackoverflow.com/questions/8249028/how-do-i-keep-my-qt-c-program-from-opening-a-console-in-windows
    set_property(TARGET ${PROJECT_NAME} PROPERTY WIN32_EXECUTABLE true)
endif()

if(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE true)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Quick Qt5::Gui Qt5::Widgets Qt5::Core Qt5::WebEngine ScreenPlaySDK)

if(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework Cocoa")
endif()
