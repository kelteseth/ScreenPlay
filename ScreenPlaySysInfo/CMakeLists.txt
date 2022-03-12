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

# Because this is a plugin, we need this for testing and development. This can be disabled when using the plugin in your project directly.
option(tst_ScreenPlaySysInfo "Builds TextProject" ON)

set(QML # cmake-format: sortable
        src/TestMain.qml)

set(QML_PLUGIN_SOURCES # cmake-format: sortable
                       src/sysinfo.cpp src/cpu.cpp src/ram.cpp src/storage.cpp src/uptime.cpp src/gpu.cpp src/ipaddress.cpp)

set(QML_PLUGIN_HEADER
    # cmake-format: sortable
    src/sysinfo.h
    src/cpu.h
    src/ram.h
    src/mathhelper.h
    src/storage.h
    src/uptime.h
    src/gpu.h
    src/ipaddress.h)

add_library(${PROJECT_NAME} STATIC)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Quick infoware)
target_include_directories(${PROJECT_NAME} PUBLIC src/)


qt_add_qml_module(
    ${PROJECT_NAME}
    URI
    ${PROJECT_NAME}
    VERSION
    1.0
    OUTPUT_DIRECTORY
    ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}
    QML_FILES
    ${QML}
    SOURCES
    ${QML_PLUGIN_SOURCES}
    ${QML_PLUGIN_HEADER})

if(${tst_ScreenPlaySysInfo})
    qt_add_executable(tst_ScreenPlaySysInfo src/TestMain.cpp)
    target_link_libraries(tst_ScreenPlaySysInfo PRIVATE Qt6::Quick ${PROJECT_NAME}plugin)
    target_compile_definitions(tst_ScreenPlaySysInfo PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
endif()
