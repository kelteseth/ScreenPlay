project(ScreenPlayShader LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

find_package(
    Qt6
    COMPONENTS Quick Core ShaderTools
    REQUIRED)

# Because this is a plugin, we need this for testing and development. This can be disabled when using the plugin in your project directly.
option(tst_ScreenPlayShader "Builds TextProject" ON)

set(QML_RESOURCES # cmake-format: sortable
    shader/lightning.frag
    shader/lightning.vert
    shader/water.frag
    shader/water.vert
    )

# https://doc.qt.io/qt-6/qtshadertools-build.html
qt6_add_shaders(${PROJECT_NAME} "${PROJECT_NAME}Shaders"
    PREFIX
         "/shaders"
    FILES
        shader/lightning.frag
        shader/lightning.vert
        shader/water.frag
        shader/water.vert
)

set(QML # cmake-format: sortable
        src/ShadertoyShader.qml
        src/TestMain.qml)

set(QML_PLUGIN_SOURCES # cmake-format: sortable
                       src/shaderlibrary.cpp)

set(QML_PLUGIN_HEADER # cmake-format: sortable
                      src/shaderlibrary.h)

add_library(${PROJECT_NAME} STATIC)
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
    ${QML_PLUGIN_HEADER}
    RESOURCES
    ${QML_RESOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Quick Qt6::Gui)

if(${tst_ScreenPlayShader})
    qt_add_executable(tst_ScreenPlayShader src/TestMain.cpp)
    target_link_libraries(tst_ScreenPlayShader PRIVATE Qt6::Quick ${PROJECT_NAME}plugin)
    target_compile_definitions(tst_ScreenPlayShader PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
endif()
