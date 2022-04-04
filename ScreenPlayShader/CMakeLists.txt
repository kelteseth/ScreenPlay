cmake_minimum_required(VERSION 3.16)
project(ScreenPlayShader LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

find_package(
    Qt6
    COMPONENTS Quick Core ShaderTools
    REQUIRED)

set(QML src/ShadertoyShader.qml src/TestMain.qml)

qt_add_library(${PROJECT_NAME} STATIC)
target_include_directories(${PROJECT_NAME} PUBLIC src/)
qt6_add_shaders(
    ${PROJECT_NAME}
    "shaders"
    BATCHABLE
    PRECOMPILE
    PREFIX
    "/ScreenPlayShader"
    FILES
    # shader/lightning.frag shader/lightning.vert
    shaders/wobble.frag
    shaders/water.frag
    shaders/water.vert)

# https://doc.qt.io/qt-6/qtshadertools-build.html

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Quick)

if(${SCREENPLAY_TESTS})
    qt_add_executable(tst_ScreenPlayShader src/TestMain.cpp)

    qt_add_qml_module(
        tst_ScreenPlayShader
        URI
        ScreenPlayShader
        VERSION
        1.0
        QML_FILES
        ${QML})

    target_link_libraries(
        tst_ScreenPlayShader
        PRIVATE Qt6::Quick
                Qt::Core
                Qt::Gui
                Qt::Qml
                Qt::Quick
                ${PROJECT_NAME})

endif()
