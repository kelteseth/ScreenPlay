cmake_minimum_required(VERSION 3.0.0 )
project(ScreenPlaySysInfo)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
find_package(Qt5Core)
find_package(Qt5Quick)
find_package(Qt5Gui)
find_package(Qt5Qml)
set(src screenplaysysinfo_plugin.cpp
	sysinfo.cpp
	cpu.cpp
	ram.cpp
	storage.cpp)
set(headers screenplaysysinfo_plugin.h
	sysinfo.h
	cpu.h
	ram.h
	mathhelper.h
	storage.h)
add_library($$qtLibraryTarget($$TARGET) MODULE ${src} ${headers})
target_link_libraries($$qtLibraryTarget($$TARGET) PRIVATE Qt5::Core Qt5::Quick Qt5::Gui Qt5::Qml)
set_target_properties($$qtLibraryTarget($$TARGET) PROPERTIES DEBUG_POSTFIX "d")
install(TARGETS $$qtLibraryTarget($$TARGET) DESTINATION Libs)
