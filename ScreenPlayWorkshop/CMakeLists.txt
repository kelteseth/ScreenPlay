add_subdirectory(SteamSDK)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 20)

find_package(
    QT NAMES Qt6 Qt5
    COMPONENTS Quick QML Widgets Gui
    REQUIRED)

find_package(
    Qt${QT_VERSION_MAJOR}
    COMPONENTS Quick QML Widgets Gui
    REQUIRED)

add_library(
    workshopplugin SHARED
    src/steamapiwrapper.h
    src/steamapiwrapper.cpp
    src/steamworkshoplistmodel.h
    src/uploadlistmodel.h
    src/steamworkshopitem.h
    src/steamworkshopitem.cpp
    src/workshop.cpp
    src/workshop.h
    src/workshopitem.h
    src/installedlistmodel.cpp
    src/installedlistmodel.h
    src/screenplayworkshop_plugin.cpp
    src/screenplayworkshop_plugin.h
    src/steamworkshop.cpp
    src/steamworkshop.h
    src/steamworkshoplistmodel.cpp
    src/steamaccount.cpp
    src/steamaccount.h
    src/steamqmlimageprovider.cpp
    src/steamqmlimageprovider.h
    qmldir
    ${qml_resources})

set(WORKSHOP_PLUGIN_DIR ${CMAKE_BINARY_DIR}/bin/workshop)
file(MAKE_DIRECTORY ${WORKSHOP_PLUGIN_DIR})

set_target_properties(workshopplugin PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${WORKSHOP_PLUGIN_DIR})
set_target_properties(workshopplugin PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${WORKSHOP_PLUGIN_DIR})
set_target_properties(workshopplugin PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${WORKSHOP_PLUGIN_DIR})

set(steam_lib_path "${CMAKE_CURRENT_SOURCE_DIR}/SteamSDK/redistributable_bin/")

if(WIN32)
    set(steam_lib "${steam_lib_path}/win64/steam_api64.lib")
elseif(APPLE)
    set(steam_lib "${steam_lib_path}/osx/libsteam_api.dylib")
elseif(UNIX)
    set(steam_lib "${steam_lib_path}/linux64/libsteam_api.so")
endif()

target_link_libraries(workshopplugin PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick ${steam_lib} ScreenPlayUtil SteamSDK)

if(WIN32)
    set(steam_bin "${steam_lib_path}/win64/steam_api64.dll")
elseif(APPLE)
    set(steam_bin "${steam_lib_path}/osx/libsteam_api.dylib")
elseif(UNIX)
    set(steam_bin "${steam_lib_path}/linux64/libsteam_api.so")
endif()

if(APPLE)
    set(workshop_install_dir ${CMAKE_BINARY_DIR}/bin/ScreenPlay.app/Contents/MacOS/Workshop)

    add_custom_target(
        build-time-make-directory
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${workshop_install_dir})

    add_custom_command(
        TARGET workshopplugin
        POST_BUILD
        COMMENT "Copying qmldir info into ScreenPlay.app bundle"
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/qmldir ${workshop_install_dir})

    add_custom_command(
        TARGET workshopplugin
        POST_BUILD
        COMMENT "Copying workshop plugin into ScreenPlay.app bundle"
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Workshop/libworkshopplugin.dylib ${workshop_install_dir})

    add_custom_command(
        TARGET workshopplugin
        POST_BUILD
        COMMENT "Copying steam library into ScreenPlay.app bundle"
        COMMAND ${CMAKE_COMMAND} -E copy ${steam_bin} ${workshop_install_dir})

    if(NOT SCREENPLAY_STEAM_DEPLOY)
        add_custom_command(
            TARGET workshopplugin
            POST_BUILD
            COMMENT "Copying steam_appid.txt into ScreenPlay.app bundle. This is for development only!"
            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/steam_appid.txt ${CMAKE_BINARY_DIR}/bin/ScreenPlay.app/Contents/MacOS/)
    endif()

else()
    if(NOT SCREENPLAY_STEAM_DEPLOY)
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/steam_appid.txt ${CMAKE_BINARY_DIR}/bin/steam_appid.txt COPYONLY)
    endif()
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/qmldir ${WORKSHOP_PLUGIN_DIR}/qmldir COPYONLY)
    configure_file(${steam_bin} ${CMAKE_BINARY_DIR}/bin/ COPYONLY)
endif()
