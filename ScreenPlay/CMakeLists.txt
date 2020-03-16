cmake_minimum_required(VERSION 3.16.0)

set (DEPENDENCIES   "openSSL"
                    "libzippp"
                    "nlohmann-json")


if(NOT DEFINED ENV{VCPKG_ROOT})
  set(VCPKG_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../Common/vcpkg)
endif()

if(NOT EXISTS ${VCPKG_ROOT})
message("Cloning vcpkg in ${VCPKG_ROOT}")
execute_process(COMMAND git clone https://github.com/Microsoft/vcpkg.git ${VCPKG_ROOT})
# execute_process(COMMAND git checkout 745a0aea597771a580d0b0f4886ea1e3a94dbca6 WORKING_DIRECTORY ${VCPKG_ROOT})
else()
# The following command has no effect if the vcpkg repository is in a detached head state.
message("Auto-updating vcpkg in ${VCPKG_ROOT}")
execute_process(COMMAND git pull WORKING_DIRECTORY ${VCPKG_ROOT})
endif()

if(NOT EXISTS ${VCPKG_ROOT}/README.md)
message(FATAL_ERROR "***** FATAL ERROR: Could not clone vcpkg *****")
endif()

if(WIN32)
set(VCPKG_EXEC ${VCPKG_ROOT}/vcpkg.exe)
set(VCPKG_BOOTSTRAP ${VCPKG_ROOT}/bootstrap-vcpkg.bat)
else()
set(VCPKG_EXEC ${VCPKG_ROOT}/vcpkg)
set(VCPKG_BOOTSTRAP ${VCPKG_ROOT}/bootstrap-vcpkg.sh)
endif()

if(NOT EXISTS ${VCPKG_EXEC})
message("Bootstrapping vcpkg in ${VCPKG_ROOT}")
execute_process(COMMAND ${VCPKG_BOOTSTRAP} WORKING_DIRECTORY ${VCPKG_ROOT})
endif()

if(NOT EXISTS ${VCPKG_EXEC})
message(FATAL_ERROR "***** FATAL ERROR: Could not bootstrap vcpkg *****")
endif()

set(CMAKE_TOOLCHAIN_FILE ${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake CACHE STRING "")

message(STATUS "***** Checking project third party dependencies in ${VCPKG_ROOT} *****")
execute_process(COMMAND ${VCPKG_EXEC} install ${DEPENDENCIES} WORKING_DIRECTORY ${VCPKG_ROOT})


project(ScreenPlay)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Qml)
find_package(Qt5Quick)
find_package(Qt5Gui)
find_package(Qt5Widgets)
find_package(Qt5Core)

find_package(OpenSSL REQUIRED)
target_link_libraries(main PRIVATE OpenSSL::SSL OpenSSL::Crypto)

find_package(libzippp CONFIG REQUIRED)
target_link_libraries(main PRIVATE libzippp::libzippp)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(main PRIVATE nlohmann_json nlohmann_json::nlohmann_json)


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
#target_compile_definitions( PRIVATE  GIT_VERSION=///"$$GIT_VERSION///")

install(TARGETS  DESTINATION Bin)
