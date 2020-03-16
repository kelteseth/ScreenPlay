cmake_minimum_required(VERSION 3.16.0)
project(ScreenPlayWallpaper)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)
find_package(Qt5Qml)
find_package(Qt5Quick)
find_package(Qt5Gui)
find_package(Qt5Widgets)
find_package(Qt5Core)
set(src main.cpp
	src/basewindow.cpp
	src/linuxwindow.cpp
	src/windowsdesktopproperties.cpp
	src/winwindow.cpp
	src/macintegration.cpp
	src/macwindow.cpp)
set(headers src/basewindow.h
	src/linuxwindow.h
	src/windowsdesktopproperties.h
	src/winwindow.h
	src/macintegration.h
	src/macbridge.h
	src/macwindow.h)
set(resources SPWResources.qrc)
add_executable(ScreenPlayWallpaper ${src} ${headers} ${resources})
target_link_libraries( ScreenPlayWallpaper Qt5::Qml Qt5::Quick Qt5::Gui Qt5::Widgets Qt5::Core)


install(TARGETS  DESTINATION Bin)
