project(
    ScreenPlayWeather
    VERSION 0.1
    LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(
    Qt6
    COMPONENTS Core Quick
    REQUIRED)

set(QML
        src/TestMain.qml)

set(QML_PLUGIN_SOURCES
                       src/screenplayweather.cpp)

set(QML_PLUGIN_HEADER
                      src/day.h src/screenplayweather.h)

qt_add_library(${PROJECT_NAME} STATIC)
target_link_libraries(${PROJECT_NAME} PUBLIC ScreenPlayUtil)
target_include_directories(${PROJECT_NAME} PUBLIC src/)

qt_add_qml_module(
    ${PROJECT_NAME}
    URI
    ${PROJECT_NAME}
    OUTPUT_DIRECTORY
    ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}
    VERSION
    1.0
    QML_FILES
    ${QML}
    SOURCES
    ${QML_PLUGIN_SOURCES}
    ${QML_PLUGIN_HEADER})

if(${SCREENPLAY_TESTS})
    qt_add_executable(tst_ScreenPlayWeather src/TestMain.cpp)
    target_link_libraries(tst_ScreenPlayWeather PRIVATE Qt6::Quick ${PROJECT_NAME}plugin)
    target_compile_definitions(tst_ScreenPlayWeather PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
endif()
