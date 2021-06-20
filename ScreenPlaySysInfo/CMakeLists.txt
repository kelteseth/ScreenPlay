project(ScreenPlaySysInfo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

find_package(infoware CONFIG REQUIRED)
find_package(
    QT NAMES Qt6 Qt5
    COMPONENTS Quick Core
    REQUIRED)

find_package(
    Qt${QT_VERSION_MAJOR}
    COMPONENTS Quick Core
    REQUIRED)

set(src
    screenplaysysinfo_plugin.cpp
    sysinfo.cpp
    cpu.cpp
    ram.cpp
    storage.cpp
    uptime.cpp
    gpu.cpp)

set(headers
    screenplaysysinfo_plugin.h
    sysinfo.h
    cpu.h
    ram.h
    mathhelper.h
    storage.h
    uptime.h
    gpu.h)

add_library(${PROJECT_NAME} SHARED ${src} ${headers})

target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick infoware)

# QML module deployment
set(URI "ScreenPlay/Sysinfo")
string(REPLACE "." "/" TARGETPATH ${URI})
if(NOT DEFINED QT_QMAKE_EXECUTABLE)
    get_target_property(QT_QMAKE_EXECUTABLE Qt${QT_VERSION_MAJOR}::qmake IMPORTED_LOCATION)
    if(NOT QT_QMAKE_EXECUTABLE)
        message(FATAL_ERROR "Cannot find qmake")
    endif()
endif()

execute_process(COMMAND ${QT_QMAKE_EXECUTABLE} -query QT_INSTALL_QML OUTPUT_VARIABLE QT_INSTALL_QML_RAW)
string(STRIP ${QT_INSTALL_QML_RAW} QT_INSTALL_QML)
set(DESTDIR "${QT_INSTALL_QML}/${TARGETPATH}")
message("DESTDIR ${DESTDIR}")

file(MAKE_DIRECTORY ${DESTDIR})

configure_file(qmldir ${DESTDIR} COPYONLY)

# Copies ScreenPlaySysInfo.* into qt qml plugins folder
add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:ScreenPlaySysInfo> ${DESTDIR}/$<TARGET_FILE_NAME:ScreenPlaySysInfo>)
