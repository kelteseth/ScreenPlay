project(ScreenPlayShader LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)
find_package(
    QT NAMES Qt6 Qt5
    COMPONENTS Quick Core
    REQUIRED)
find_package(
    Qt${QT_VERSION_MAJOR}
    COMPONENTS Quick Core
    REQUIRED)

set(src screenplayshader_plugin.cpp shaderlibrary.cpp)

set(headers screenplayshader_plugin.h shaderlibrary.h)

set(shader shader.qrc)

add_library(${PROJECT_NAME} SHARED ${src} ${headers} ${shader})

target_include_directories(${PROJECT_NAME} PUBLIC inc)

# QML module deployment
set(URI "ScreenPlay/Shader")
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

if(APPLE)
    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMENT "Copying into ScreenPlay.app bundle"
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/bin/$<TARGET_FILE_NAME:${PROJECT_NAME}>
                ${CMAKE_BINARY_DIR}/bin/ScreenPlay.app/Contents/MacOS/$<TARGET_FILE_NAME:${PROJECT_NAME}>)
else()
    # Copies ScreenPlayShader.* into qt qml plugins folder
    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:ScreenPlayShader> ${DESTDIR}/$<TARGET_FILE_NAME:ScreenPlayShader>)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick Qt${QT_VERSION_MAJOR}::Gui)
