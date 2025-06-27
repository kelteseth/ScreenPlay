# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
project(
    ScreenPlayShader
    VERSION 0.1
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(
    Qt6
    COMPONENTS Quick Core ShaderTools
    REQUIRED)

qt_standard_project_setup(REQUIRES 6.8)

set(QML src/ShadertoyShader.qml src/TestMain.qml)

set(QML_PLUGIN_SOURCES src/shaderlibrary.cpp)

set(QML_PLUGIN_HEADER src/shaderlibrary.h)

set(SHADER

    # cmake-format: sort
    shaders/lightning.frag
    shaders/lightning.vert
    shaders/water.frag
    shaders/water.vert
    shaders/wobble.frag)

set(RESOURCES

    # cmake-format: sort
    assets/mask_01.png
    assets/Shadertoy_Bayer.png
    assets/Shadertoy_Gray_Noise_Medium.png
    assets/Shadertoy_Lichen.jpg
    assets/test_image_andras-vas-Bd7gNnWJBkU-unsplash.jpg)

qt_add_library(ScreenPlayShaderLib STATIC)
target_compile_features(ScreenPlayShaderLib PRIVATE cxx_std_23)
target_link_libraries(ScreenPlayShaderLib PUBLIC Qt6::Core Qt6::Quick ScreenPlayCoreLib ScreenPlayCoreLibplugin)
target_include_directories(ScreenPlayShaderLib PUBLIC src/)

qt_add_qml_module(
    ScreenPlayShaderLib
    URI
    ScreenPlayShader
    VERSION
    1.0
    SOURCES
    ${QML_PLUGIN_SOURCES}
    ${QML_PLUGIN_HEADER}
    QML_FILES
    ${QML}
    RESOURCES
    ${RESOURCES})

# https://doc.qt.io/qt-6/qtshadertools-build.html
qt6_add_shaders(
    ScreenPlayShaderLib
    "shaders"
    BATCHABLE
    PRECOMPILE
    PREFIX
    "/"
    FILES
    ${SHADER})

if(${SCREENPLAY_TESTS})
    qt_add_executable(tst_ScreenPlayShader src/tst_ScreenPlayShader.cpp)
    target_link_libraries(
        tst_ScreenPlayShader
        PRIVATE Qt::Quick
        ScreenPlayShaderLib
        ScreenPlayShaderLibplugin
        ScreenPlayCoreLib
        ScreenPlayCoreLibplugin)
endif()
