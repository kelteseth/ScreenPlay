project(workshopplugin LANGUAGES CXX)

add_subdirectory(SteamSDK)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 20)

find_package(
    Qt6
    COMPONENTS Quick  Widgets Gui
    REQUIRED)

set(SOURCES
    # cmake-format: sortable
    src/steamapiwrapper.cpp
    src/steamworkshopitem.cpp
    src/workshop.cpp
    src/installedlistmodel.cpp
    src/screenplayworkshop_plugin.cpp
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
    src/screenplayworkshop_plugin.h
    src/steamworkshop.h
    src/steamaccount.h
    src/steamqmlimageprovider.h)

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADER})

set(WORKSHOP_PLUGIN_DIR ${CMAKE_BINARY_DIR}/bin/workshop)
file(MAKE_DIRECTORY ${WORKSHOP_PLUGIN_DIR})

set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${WORKSHOP_PLUGIN_DIR})
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${WORKSHOP_PLUGIN_DIR})
set_target_properties(${PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${WORKSHOP_PLUGIN_DIR})

set(STEAM_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/SteamSDK/redistributable_bin/")

if(WIN32)
    set(STEAM_LIB "${STEAM_LIB_PATH}/win64/steam_api64.lib")
    set(STEAM_BIN "${STEAM_LIB_PATH}/win64/steam_api64.dll")
elseif(APPLE)
    set(STEAM_LIB "${STEAM_LIB_PATH}/osx/libsteam_api.dylib")
    set(STEAM_BIN ${STEAM_LIB})
elseif(UNIX)
    set(STEAM_LIB "${STEAM_LIB_PATH}/linux64/libsteam_api.so")
    set(STEAM_BIN ${STEAM_LIB})
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Quick ${STEAM_LIB} ScreenPlayUtil SteamSDK)

if(APPLE)
    set(workshop_install_dir ${CMAKE_BINARY_DIR}/bin/ScreenPlay.app/Contents/MacOS/Workshop)

    add_custom_target(
        build-time-make-directory
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${workshop_install_dir})

    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMENT "Copying workshop plugin into ScreenPlay.app bundle"
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Workshop/libworkshopplugin.dylib ${workshop_install_dir})

    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMENT "Copying steam library into ScreenPlay.app bundle"
        COMMAND ${CMAKE_COMMAND} -E copy ${STEAM_BIN} ${workshop_install_dir})

    if(${SCREENPLAY_STEAM})
        add_custom_command(
            TARGET ${PROJECT_NAME}
            POST_BUILD
            COMMENT "Copying steam_appid.txt into ScreenPlay.app bundle. This is for development only!"
            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/steam_appid.txt
                    ${CMAKE_BINARY_DIR}/bin/ScreenPlay.app/Contents/MacOS/)
    endif()

else()
    if(${SCREENPLAY_STEAM})
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/steam_appid.txt ${CMAKE_BINARY_DIR}/bin/steam_appid.txt COPYONLY)
    configure_file(${STEAM_BIN} ${CMAKE_BINARY_DIR}/bin/ COPYONLY)
    endif()
endif()
