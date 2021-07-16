add_subdirectory(SteamSDK)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 20)


find_package(
    QT NAMES Qt6 Qt5
    COMPONENTS Quick
    QML
    Widgets
    Gui
    REQUIRED)

find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Quick
             QML
             Widgets
             Gui
  REQUIRED)

add_library(
    workshopplugin
    SHARED
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
    ${qml_resources}
)

set_target_properties(workshopplugin PROPERTIES CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/workshop)
set_target_properties(workshopplugin PROPERTIES CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/workshop)
set_target_properties(workshopplugin PROPERTIES CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/workshop)

set(steam_lib_path "${CMAKE_CURRENT_SOURCE_DIR}/SteamSDK/redistributable_bin/")

if(WIN32)
  set(steam_lib "${steam_lib_path}/win64/steam_api64.lib")
elseif(APPLE)
  set(steam_lib "${steam_lib_path}/osx/libsteam_api.dylib")
elseif(UNIX)
  set(steam_lib "${steam_lib_path}/linux64/libsteam_api.so")
endif()

target_link_libraries(
    workshopplugin
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Quick
        ${steam_lib}
        ScreenPlayUtil
        SteamSDK
)

if(WIN32)
  set(steam_bin "${steam_lib_path}/win64/steam_api64.dll")
elseif(APPLE)
  set(steam_bin "${steam_lib_path}/osx/libsteam_api.dylib")
elseif(UNIX)
  set(steam_bin "${steam_lib_path}/linux64/libsteam_api.so")
endif()


configure_file(${CMAKE_CURRENT_SOURCE_DIR}/qmldir ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/qmldir COPYONLY)


add_custom_command(
    TARGET workshopplugin POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${steam_bin}
    ${CMAKE_BINARY_DIR}/bin/ )

add_custom_command(
    TARGET workshopplugin POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_CURRENT_SOURCE_DIR}/steam_appid.txt
    ${CMAKE_BINARY_DIR}/bin/ )

