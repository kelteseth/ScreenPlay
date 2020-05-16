project(ScreenPlayWidget)

set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

find_package(
  Qt5
  COMPONENTS Quick
             Widgets
             Gui
             WebEngine
  REQUIRED)

set(src main.cpp
	src/widgetwindow.cpp)

set(headers
    src/widgetwindow.h)

set(resources
    SPWidgetResources.qrc)

add_executable(ScreenPlayWidget ${src} ${headers} ${resources})

target_link_libraries( ScreenPlayWidget
    PRIVATE
    Qt5::Quick
    Qt5::Gui
    Qt5::Widgets
    Qt5::Core
    ScreenPlaySDK
    )

