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

qt_standard_project_setup(REQUIRES 6.8)

set(QML qml/TestMain.qml)

set(QML_PLUGIN_SOURCES

    # cmake-format: sort
    src/cpu.cpp
    src/gpu.cpp
    src/ipaddress.cpp
    src/networkinterface.cpp
    src/ram.cpp
    src/storage.cpp
    src/sysinfo.cpp
    src/uptime.cpp)

if(WIN32)
    list(APPEND QML_PLUGIN_SOURCES src/networkinterface_win.cpp)
endif()

set(QML_PLUGIN_HEADER

    # cmake-format: sort
    inc/public/ScreenPlaySysInfo/cpu.h
    inc/public/ScreenPlaySysInfo/gpu.h
    inc/public/ScreenPlaySysInfo/ipaddress.h
    inc/public/ScreenPlaySysInfo/networkinterface.h
    inc/public/ScreenPlaySysInfo/ram.h
    inc/public/ScreenPlaySysInfo/storage.h
    inc/public/ScreenPlaySysInfo/sysinfo.h
    inc/public/ScreenPlaySysInfo/uptime.h)

if(WIN32)
    list(APPEND QML_PLUGIN_HEADER inc/private/networkinterface_win.h)
endif()

qt_add_library(ScreenPlaySysInfoLib STATIC)
target_compile_features(ScreenPlaySysInfoLib PRIVATE cxx_std_23)

target_link_libraries(
    ScreenPlaySysInfoLib
    PRIVATE Qt::NetworkPrivate
    Qt6::Core
    Qt6::Quick
    infoware)

target_include_directories(ScreenPlaySysInfoLib PUBLIC inc/public)

# Add full inc/public/ScreenPlaySysInfo path for moc includes
target_include_directories(ScreenPlaySysInfoLib PRIVATE inc/private inc/public/ScreenPlaySysInfo)

if(WIN32)
    find_package(Qt6 COMPONENTS AxContainer)
    target_link_libraries(ScreenPlaySysInfoLib PUBLIC Qt6::AxContainer ole32.lib winmm.lib)
    target_compile_definitions(ScreenPlaySysInfoLib PUBLIC WIN32_LEAN_AND_MEAN _WINSOCKAPI_ NOMINMAX)
endif()

qt_add_qml_module(
    ScreenPlaySysInfoLib
    URI
    ScreenPlaySysInfo
    VERSION
    1.0
    QML_FILES
    ${QML}
    SOURCES
    ${QML_PLUGIN_SOURCES}
    ${QML_PLUGIN_HEADER})

if(${SCREENPLAY_TESTS})
    qt_add_executable(tst_ScreenPlaySysInfo src/tst_ScreenPlaySysInfo.cpp)
    target_link_libraries(
        tst_ScreenPlaySysInfo
        PRIVATE Qt6::Quick
        ScreenPlaySysInfoLib
        ScreenPlaySysInfoLibplugin)
endif()
