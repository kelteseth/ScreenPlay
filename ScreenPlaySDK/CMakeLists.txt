project(ScreenPlaySDK)

set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

find_package(
  Qt5
  COMPONENTS Quick
             Network
             Core
  REQUIRED)

set(src screenplay-sdk_plugin.cpp
        screenplaysdk.cpp)

set(headers screenplay-sdk_plugin.h
        screenplaysdk.h)

add_library(${PROJECT_NAME}  ${src} ${headers})

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Qt5::Core
    Qt5::Quick
    Qt5::Gui
    Qt5::Network)
