# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
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
find_package(Qt6 COMPONENTS AxContainer)

set(QML qml/TestMain.qml)

set(QML_PLUGIN_SOURCES
    # cmake-format: sort
    src/networkinterface.cpp
    src/cpu.cpp
    src/gpu.cpp
    src/ipaddress.cpp
    src/ram.cpp
    src/storage.cpp
    src/sysinfo.cpp
    src/uptime.cpp)

if(WIN32)
    list(APPEND QML_PLUGIN_SOURCES src/networkinterface_win.cpp)
endif()

set(QML_PLUGIN_HEADER
    # cmake-format: sort
    src/networkinterface.h
    src/cpu.h
    src/gpu.h
    src/ipaddress.h
    src/ram.h
    src/storage.h
    src/sysinfo.h
    src/uptime.h)

if(WIN32)
    list(APPEND QML_PLUGIN_HEADER src/networkinterface_win.h)
endif()

qt_add_library(${PROJECT_NAME} STATIC)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt::NetworkPrivate
    Qt6::Core
    Qt6::Quick
    infoware)

target_include_directories(${PROJECT_NAME} PUBLIC src/)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::AxContainer ole32.lib winmm.lib)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        WIN32_LEAN_AND_MEAN
        _WINSOCKAPI_
        NOMINMAX
    )
endif()

qt_add_qml_module(
    ${PROJECT_NAME}
    URI
    ${PROJECT_NAME}
    VERSION
    1.0
    OUTPUT_DIRECTORY
    ${SCREENPLAY_QML_MODULES_PATH}/${PROJECT_NAME}
    RESOURCE_PREFIX
    /qml
    QML_FILES
    ${QML}
    SOURCES
    ${QML_PLUGIN_SOURCES}
    ${QML_PLUGIN_HEADER}
)

if(${SCREENPLAY_TESTS})
    qt_add_executable(tst_ScreenPlaySysInfo src/TestMain.cpp)
    target_link_libraries(tst_ScreenPlaySysInfo PRIVATE Qt6::Quick ${PROJECT_NAME}plugin)
endif()
