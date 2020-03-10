cmake_minimum_required(VERSION 3.0.0 )
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
add_executable( ${src} ${headers} ${resources})
target_link_libraries( PRIVATE Qt5::Qml Qt5::Quick Qt5::Gui Qt5::Widgets Qt5::Core)
target_compile_definitions( PRIVATE  QT_DEPRECATED_WARNINGS)
set_target_properties( PROPERTIES DEBUG_POSTFIX "d")
install(TARGETS  DESTINATION Bin)
