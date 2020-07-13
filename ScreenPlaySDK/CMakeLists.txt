project(ScreenPlaySDK)

set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

find_package(
  Qt5
  COMPONENTS Quick
             Network
             Core
  REQUIRED)

set(src src/screenplay-sdk_plugin.cpp
        src/screenplaysdk.cpp)

set(headers
        inc/screenplay-sdk_plugin.h
        inc/screenplaysdk.h)

add_library(${PROJECT_NAME}  ${src} ${headers})

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Qt5::Core
    Qt5::Quick
    Qt5::Gui
    Qt5::Network)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC  inc
    )
