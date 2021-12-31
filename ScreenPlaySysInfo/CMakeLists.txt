project(ScreenPlaySysInfo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

find_package(infoware CONFIG REQUIRED)

find_package(
    Qt6
    COMPONENTS Quick Core
    REQUIRED)

set(SOURCES
    # cmake-format: sortable
    screenplaysysinfo_plugin.cpp
    sysinfo.cpp
    cpu.cpp
    ram.cpp
    storage.cpp
    uptime.cpp
    gpu.cpp)

set(HEADER
    # cmake-format: sortable
    screenplaysysinfo_plugin.h
    sysinfo.h
    cpu.h
    ram.h
    mathhelper.h
    storage.h
    uptime.h
    gpu.h)

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADER})

qt_add_qml_module(
    ${PROJECT_NAME}
    OUTPUT_DIRECTORY
    ${CMAKE_BINARY_DIR}/bin/SysInfo
    URI
    ${PROJECT_NAME}
    VERSION
    1.0)

target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Quick infoware)
