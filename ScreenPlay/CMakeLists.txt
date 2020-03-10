cmake_minimum_required(VERSION 3.0.0 )
project(ScreenPlay)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)
find_package(Qt5Qml)
find_package(Qt5Quick)
find_package(Qt5Gui)
find_package(Qt5Widgets)
find_package(Qt5Core)
set(src main.cpp
        app.cpp
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
target_link_libraries(ScreenPlay  Qt5::Qml Qt5::Quick Qt5::Gui Qt5::Widgets Qt5::Core)
#target_compile_definitions( PRIVATE  GIT_VERSION=\\\"$$GIT_VERSION\\\")

install(TARGETS  DESTINATION Bin)
