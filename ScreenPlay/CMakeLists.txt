cmake_minimum_required(VERSION 3.16.0)

project(ScreenPlay)

set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

find_package(
  Qt5
  COMPONENTS Quick
             QuickCompiler
             Widgets
             Gui
             WebEngine
  REQUIRED)

find_package(ZLIB REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(libzippp CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)


set(src main.cpp
        app.cpp
        ../Common/qt-google-analytics/ganalytics.cpp
        src/globalvariables.cpp
        src/createimportvideo.cpp
        src/installedlistmodel.cpp
        src/monitorlistmodel.cpp
        src/screenplaywallpaper.cpp
        src/screenplaywidget.cpp
        src/settings.cpp
        src/profilelistmodel.cpp
        src/installedlistfilter.cpp
        src/sdkconnector.cpp
        src/projectsettingslistmodel.cpp
        src/screenplaymanager.cpp
        src/util.cpp
        src/create.cpp)

set(headers app.h
        ../Common/qt-google-analytics/ganalytics.h
        src/globalvariables.h
        src/createimportvideo.h
        src/installedlistmodel.h
        src/monitorlistmodel.h
        src/screenplaywallpaper.h
        src/screenplaywidget.h
        src/settings.h
        src/profilelistmodel.h
        src/profile.h
        src/projectfile.h
        src/installedlistfilter.h
        src/sdkconnector.h
        src/projectsettingslistitem.h
        src/projectsettingslistmodel.h
        src/screenplaymanager.h
        src/util.h
        src/create.h)

qt5_add_big_resources(resources  resources.qrc)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(qml qml.qrc)
else()
    qtquick_compiler_add_resources(qml qml.qrc )
endif()


add_executable(${PROJECT_NAME} ${src} ${headers} ${resources} ${qml})

# Disable console window on Windows
# https://stackoverflow.com/questions/8249028/how-do-i-keep-my-qt-c-program-from-opening-a-console-in-windows
set_property(TARGET ${PROJECT_NAME} PROPERTY WIN32_EXECUTABLE true)

target_compile_definitions(${PROJECT_NAME} PRIVATE  "GIT_VERSION=${GIT_VERSION}")

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Qt5::Quick
    Qt5::Gui
    Qt5::Widgets
    Qt5::Core
    Qt5::WebEngine
    nlohmann_json::nlohmann_json
    libzippp::libzippp
    ScreenPlaySDK
    QTBreakpadplugin)

target_include_directories(${PROJECT_NAME}
    PRIVATE
    ${QT_TELEMTRY_INCLUDE}
    ${QT_BREAKPAD_INCLUDE})

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/assets/fonts)
configure_file(assets/fonts/NotoSansCJKkr-Regular.otf ${CMAKE_BINARY_DIR}/bin/assets/fonts COPYONLY)


execute_process(COMMAND git log --pretty=format:'%h' -n 1
                OUTPUT_VARIABLE GIT_VERSION
                ERROR_QUIET)
