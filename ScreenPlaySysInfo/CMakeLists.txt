project(ScreenPlaySysInfo)

set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

find_package(
  Qt5
  COMPONENTS Quick
             QML
             Core
  REQUIRED)


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

add_library(${PROJECT_NAME}  ${src} ${headers})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core Qt5::Quick Qt5::Qml)


