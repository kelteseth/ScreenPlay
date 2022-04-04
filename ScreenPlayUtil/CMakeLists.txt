project(ScreenPlayUtil LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

find_package(
    Qt6
    COMPONENTS Core Quick
    REQUIRED)

set(SOURCES inc/public/ScreenPlayUtil/httpfileserver.cpp src/contenttypes.cpp src/util.cpp)

set(HEADER
    inc/public/ScreenPlayUtil/AutoPropertyHelpers.h
    inc/public/ScreenPlayUtil/ConstRefPropertyHelpers.h
    inc/public/ScreenPlayUtil/contenttypes.h
    inc/public/ScreenPlayUtil/EnumClassHelper.h
    inc/public/ScreenPlayUtil/HelpersCommon.h
    inc/public/ScreenPlayUtil/httpfileserver.h
    inc/public/ScreenPlayUtil/ListPropertyHelper.h
    inc/public/ScreenPlayUtil/projectfile.h
    inc/public/ScreenPlayUtil/PropertyHelpers.h
    inc/public/ScreenPlayUtil/PtrPropertyHelpers.h
    inc/public/ScreenPlayUtil/SingletonHelper.h
    inc/public/ScreenPlayUtil/util.h)

qt_add_library(
    ${PROJECT_NAME}
    STATIC
    ${SOURCES}
    ${HEADER})

find_path(CPP_HTTPLIB_INCLUDE_DIRS "httplib.h")
target_include_directories(${PROJECT_NAME} PUBLIC ${CPP_HTTPLIB_INCLUDE_DIRS})

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC inc/public/
    PRIVATE src/)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Quick)

if(WIN32)
    # Used for query windows monitor data
    target_link_libraries(${PROJECT_NAME} PUBLIC shcore.lib)
endif()
