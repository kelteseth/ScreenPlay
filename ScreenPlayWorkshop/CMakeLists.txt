project(ScreenPlayWorkshop LANGUAGES CXX)

add_subdirectory(SteamSDK)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 20)

find_package(
    Qt6
    COMPONENTS Quick QML Widgets Gui
    REQUIRED)

set(SOURCES
    # cmake-format: sortable
    src/steamapiwrapper.cpp
    src/steamworkshopitem.cpp
    src/workshop.cpp
    src/installedlistmodel.cpp
    src/steamworkshop.cpp
    src/steamworkshoplistmodel.cpp
    src/steamaccount.cpp
    src/steamqmlimageprovider.cpp)

set(HEADER
    # cmake-format: sortable
    src/steamapiwrapper.h
    src/steamworkshoplistmodel.h
    src/uploadlistmodel.h
    src/steamworkshopitem.h
    src/workshop.h
    src/workshopitem.h
    src/installedlistmodel.h
    src/steamworkshop.h
    src/steamaccount.h
    src/steamqmlimageprovider.h)

set(STEAM_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/SteamSDK/redistributable_bin/")

if(WIN32)
    set(WORKSHOP_PLUGIN_DIR ${CMAKE_BINARY_DIR}/bin/Workshop)
    set(STEAM_LIB "${STEAM_LIB_PATH}/win64/steam_api64.lib")
    set(STEAM_BIN "${STEAM_LIB_PATH}/win64/steam_api64.dll")
elseif(APPLE)
    set(WORKSHOP_PLUGIN_DIR ${CMAKE_BINARY_DIR}/bin/ScreenPlay.app/Contents/MacOS/Workshop)
    set(STEAM_LIB "${STEAM_LIB_PATH}/osx/libsteam_api.dylib")
    set(STEAM_BIN ${STEAM_LIB})
elseif(UNIX)
    set(WORKSHOP_PLUGIN_DIR ${CMAKE_BINARY_DIR}/bin/Workshop)
    set(STEAM_LIB "${STEAM_LIB_PATH}/linux64/libsteam_api.so")
    set(STEAM_BIN ${STEAM_LIB})
endif()

qt_add_qml_module(
    ${PROJECT_NAME}
    OUTPUT_DIRECTORY ${WORKSHOP_PLUGIN_DIR}
    URI "Workshop"
    SOURCES  ${SOURCES} ${HEADER}
    VERSION 1.0)

if(APPLE)
    if(${SCREENPLAY_STEAM})
        add_custom_command(
            TARGET ${PROJECT_NAME}
            POST_BUILD
            COMMENT "Copying steam_appid.txt into ScreenPlay.app bundle. This is for development only!"
            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/steam_appid.txt
                    ${CMAKE_BINARY_DIR}/bin/ScreenPlay.app/Contents/MacOS/)

        add_custom_command(
            TARGET ${PROJECT_NAME}
            POST_BUILD
            COMMENT "Copying libsteam_api.dylib into ScreenPlay.app bundle."
            COMMAND ${CMAKE_COMMAND} -E copy ${STEAM_LIB}
                    ${WORKSHOP_PLUGIN_DIR})
    endif()
else()
    if(${SCREENPLAY_STEAM})
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/steam_appid.txt ${CMAKE_BINARY_DIR}/bin/steam_appid.txt COPYONLY)
        configure_file(${STEAM_BIN} ${CMAKE_BINARY_DIR}/bin/ COPYONLY)
    endif()
endif()


# Needed by the automatic generated target missing includes
# https://github.com/qt/qtdeclarative/blob/7a7064e14f094e843e1ee832cc927e86f887621a/src/qml/Qt6QmlMacros.cmake#L2042
target_include_directories(${PROJECT_NAME} PUBLIC src/)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Quick ${STEAM_LIB} ScreenPlayUtil SteamSDK SteamSDKQtEnums)
