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


qt_add_library(${PROJECT_NAME} STATIC)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Quick)
target_include_directories(${PROJECT_NAME} PUBLIC src/ )

qt_add_qml_module(
    ${PROJECT_NAME}
    OUTPUT_DIRECTORY
    ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}
    URI
    ${PROJECT_NAME}
    VERSION
    1.0
    SOURCES
    ${QML_PLUGIN_SOURCES}
    ${QML_PLUGIN_HEADER}
    QML_FILES
    ${QML})

# https://doc.qt.io/qt-6/qtshadertools-build.html
qt6_add_shaders(
    ${PROJECT_NAME}
    "shaders"
    BATCHABLE
    PRECOMPILE
    PREFIX
    "/"
    FILES
    # shader/lightning.frag shader/lightning.vert
    shaders/wobble.frag
    shaders/water.frag
    shaders/water.vert)

if(${SCREENPLAY_TESTS})
    qt_add_executable(tst_ScreenPlayShader src/TestMain.cpp Resources.qrc)
    target_link_libraries(tst_ScreenPlayShader PRIVATE Qt::Quick ${PROJECT_NAME}plugin )
    target_compile_definitions(tst_ScreenPlayShader PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
endif()
