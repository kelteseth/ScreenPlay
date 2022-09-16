project(
    ScreenPlaySysInfo
    VERSION 0.1
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(infoware CONFIG REQUIRED)

find_package(
    Qt6
    COMPONENTS Quick Core
    REQUIRED)

set(QML qml/TestMain.qml)

set(QML_PLUGIN_SOURCES
    src/cpu.cpp
    src/gpu.cpp
    src/ipaddress.cpp
    src/ram.cpp
    src/storage.cpp
    src/sysinfo.cpp
    src/uptime.cpp)

set(QML_PLUGIN_HEADER
    src/cpu.h
    src/gpu.h
    src/ipaddress.h
    src/mathhelper.h
    src/ram.h
    src/storage.h
    src/sysinfo.h
    src/uptime.h)

qt_add_library(${PROJECT_NAME} STATIC)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Quick infoware)
target_include_directories(${PROJECT_NAME} PUBLIC src/)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE ole32.lib winmm.lib)
endif()

qt_add_qml_module(
    ${PROJECT_NAME}
    URI
    ${PROJECT_NAME}
    VERSION
    1.0
    OUTPUT_DIRECTORY
    ${SCREENPLAY_QML_MODULES_PATH}/${PROJECT_NAME}
    RESOURCE_PREFIX /qml
    QML_FILES
    ${QML}
    SOURCES
    ${QML_PLUGIN_SOURCES}
    ${QML_PLUGIN_HEADER})

if(${SCREENPLAY_TESTS})
    qt_add_executable(tst_ScreenPlaySysInfo src/TestMain.cpp)
    target_link_libraries(tst_ScreenPlaySysInfo PRIVATE Qt6::Quick ${PROJECT_NAME}plugin)
endif()
