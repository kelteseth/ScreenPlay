# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
project(
    ScreenPlayShader
    VERSION 0.1
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(
    Qt6
    COMPONENTS Quick Core ShaderTools
    REQUIRED)

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

qt_add_library(${PROJECT_NAME} STATIC)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
target_link_libraries(${PROJECT_NAME} PUBLIC Qt6::Core Qt6::Quick ScreenPlayCoreplugin)
target_include_directories(${PROJECT_NAME} PUBLIC src/)

qt_add_qml_module(
    ${PROJECT_NAME}
    OUTPUT_DIRECTORY
    ${SCREENPLAY_QML_MODULES_PATH}/${PROJECT_NAME}
    URI
    ${PROJECT_NAME}
    RESOURCE_PREFIX
    /qml
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
    ${PROJECT_NAME}
    "shaders"
    BATCHABLE
    PRECOMPILE
    PREFIX
    "/"
    FILES
    ${SHADER})

if(${SCREENPLAY_TESTS})
    qt_add_executable(tst_ScreenPlayShader src/TestMain.cpp)
    target_link_libraries(tst_ScreenPlayShader PRIVATE Qt::Quick ${PROJECT_NAME}plugin)

    if(APPLE)
        set_target_properties(tst_ScreenPlayShader PROPERTIES RUNTIME_OUTPUT_DIRECTORY
            "${CMAKE_BINARY_DIR}/bin/ScreenPlay.app/Contents/MacOS/")
    endif()
endif()
