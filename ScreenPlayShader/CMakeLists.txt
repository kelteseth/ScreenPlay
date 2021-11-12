project(ScreenPlayShader LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

find_package(
    Qt6
    COMPONENTS Quick Core
    REQUIRED)

set(SOURCES # cmake-format: sortable
            screenplayshader_plugin.cpp shaderlibrary.cpp)

set(HEADER # cmake-format: sortable
           screenplayshader_plugin.h shaderlibrary.h)

qt_add_resources(RESOURCES Resources.qrc)

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADER} ${RESOURCES})

qt_add_qml_module(
    ${PROJECT_NAME}
    URI
    ${PROJECT_NAME}
    VERSION
    1.0
    QML_FILES
    ShadertoyShader.qml)

target_include_directories(${PROJECT_NAME} PRIVATE inc)

target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Quick Qt6::Gui)
