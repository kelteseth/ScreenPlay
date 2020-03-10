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
set(src $$PWD/main.cpp
	$$PWD/app.cpp
	$$PWD/src/createimportvideo.cpp
	$$PWD/src/installedlistmodel.cpp
	$$PWD/src/monitorlistmodel.cpp
	$$PWD/src/screenplaywallpaper.cpp
	$$PWD/src/screenplaywidget.cpp
	$$PWD/src/settings.cpp
	$$PWD/src/profilelistmodel.cpp
	$$PWD/src/installedlistfilter.cpp
	$$PWD/src/sdkconnector.cpp
	$$PWD/src/projectsettingslistmodel.cpp
	$$PWD/src/screenplaymanager.cpp
	$$PWD/src/util.cpp
	$$PWD/src/create.cpp)
set(headers $$PWD/app.h
	$$PWD/src/globalvariables.h
	$$PWD/src/createimportvideo.h
	$$PWD/src/installedlistmodel.h
	$$PWD/src/monitorlistmodel.h
	$$PWD/src/screenplaywallpaper.h
	$$PWD/src/screenplaywidget.h
	$$PWD/src/settings.h
	$$PWD/src/profilelistmodel.h
	$$PWD/src/profile.h
	$$PWD/src/projectfile.h
	$$PWD/src/installedlistfilter.h
	$$PWD/src/sdkconnector.h
	$$PWD/src/projectsettingslistitem.h
	$$PWD/src/projectsettingslistmodel.h
	$$PWD/src/screenplaymanager.h
	$$PWD/src/util.h
	$$PWD/src/create.h)
set(resources $$PWD/Resources.qrc)
add_executable( ${src} ${headers} ${resources})
target_link_libraries( PRIVATE Qt5::Qml Qt5::Quick Qt5::Gui Qt5::Widgets Qt5::Core)
target_include_directories( PUBLIC  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/$$PWD/src/> $<INSTALL_INTERFACE:$$PWD/src/>)
target_include_directories( PUBLIC  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/$$PWD/../Common/vcpkg/installed/x64-windows/include> $<INSTALL_INTERFACE:$$PWD/../Common/vcpkg/installed/x64-windows/include>)
target_include_directories( PUBLIC  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/$$PWD/ThirdParty/steam/> $<INSTALL_INTERFACE:$$PWD/ThirdParty/steam/>)
target_include_directories( PUBLIC  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/$$PWD/../Common/vcpkg/installed/x64-linux/include> $<INSTALL_INTERFACE:$$PWD/../Common/vcpkg/installed/x64-linux/include>)
target_compile_definitions( PRIVATE  QT_DEPRECATED_WARNINGS)
target_compile_definitions( PRIVATE  GIT_VERSION=\\\"$$GIT_VERSION\\\")
set_target_properties( PROPERTIES DEBUG_POSTFIX "d")
install(TARGETS  DESTINATION Bin)
