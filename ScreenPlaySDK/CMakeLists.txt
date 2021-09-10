project(ScreenPlaySDK LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

find_package(
    Qt6
    COMPONENTS Quick Network Core
    REQUIRED)

set(src src/screenplay-sdk_plugin.cpp src/screenplaysdk.cpp)

set(headers inc/screenplay-sdk_plugin.h inc/screenplaysdk.h)

add_library(${PROJECT_NAME} ${src} ${headers})

target_include_directories(${PROJECT_NAME} PUBLIC inc)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Quick Qt6::Gui Qt6::Network)
