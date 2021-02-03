project(ScreenPlayUtil LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

find_package(
    Qt5
    COMPONENTS  Core
    REQUIRED)

set(SOURCES
    src/util.cpp
    src/contenttypes.cpp
    )

set(HEADER
    inc/public/ScreenPlayUtil/util.h
    inc/public/ScreenPlayUtil/contenttypes.h
    inc/public/ScreenPlayUtil/projectfile.h
    )

add_library(${PROJECT_NAME} STATIC  ${SOURCES} ${HEADER})

target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core )

target_include_directories(${PROJECT_NAME} PUBLIC inc/public/ PRIVATE src/)
