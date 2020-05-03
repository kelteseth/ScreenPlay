cmake_minimum_required(VERSION 3.17)

project(ScreenPlay)

set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

find_package(
  Qt5
  COMPONENTS Quick
             QML
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

set(resources Resources.qrc)

add_executable(ScreenPlay ${src} ${headers} ${resources})

target_compile_definitions(${PROJECT_NAME} PRIVATE  "GIT_VERSION=${GIT_VERSION}")

target_link_libraries(ScreenPlay PRIVATE
    Qt5::Qml
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

install(FILES
    assets/fonts/NotoSansCJKkr-Regular.otf
    DESTINATION
    ${CMAKE_BINARY_DIR}/assets/fonts/ )



execute_process(COMMAND git log --pretty=format:'%h' -n 1
                OUTPUT_VARIABLE GIT_VERSION
                ERROR_QUIET)
