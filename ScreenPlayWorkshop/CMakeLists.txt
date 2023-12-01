# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
project(
    ScreenPlayWorkshop
    VERSION 0.1
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(
    Qt6
    COMPONENTS Core Quick QuickControls2
    REQUIRED)

set(QML
    # cmake-format: sort
    qml/Background.qml
    qml/Forum.qml
    qml/Navigation.qml
    qml/PopupOffline.qml
    qml/ScreenPlayItem.qml
    qml/ScreenPlayItemImage.qml
    qml/Sidebar.qml
    qml/SteamProfile.qml
    qml/SteamWorkshop.qml
    qml/SteamWorkshopStartPage.qml
    qml/TestMain.qml
    qml/upload/PopupSteamWorkshopAgreement.qml
    qml/upload/UploadProject.qml
    qml/upload/UploadProjectBigItem.qml
    qml/upload/UploadProjectItem.qml
    qml/Workshop.qml
    qml/WorkshopInstalled.qml
    qml/WorkshopItem.qml)

set(SOURCES
    # cmake-format: sort
    src/installedlistmodel.cpp
    src/screenplayworkshop.cpp
    src/steamaccount.cpp
    src/steamapiwrapper.cpp
    src/steamqmlimageprovider.cpp
    src/steamworkshop.cpp
    src/steamworkshopitem.cpp
    src/steamworkshoplistmodel.cpp)

set(HEADER
    # cmake-format: sort
    src/installedlistmodel.h
    src/screenplayworkshop.h
    src/steamaccount.h
    src/steamapiwrapper.h
    src/steamqmlimageprovider.h
    src/steamworkshop.h
    src/steamworkshopitem.h
    src/steamworkshoplistmodel.h
    src/uploadlistmodel.h
    src/workshopitem.h)

set(RESOURCES
    # cmake-format: sort
    assets/icons/icon_account_circle.svg
    assets/icons/icon_arrow_left.svg
    assets/icons/icon_arrow_right.svg
    assets/icons/icon_close.svg
    assets/icons/icon_download.svg
    assets/icons/icon_file_upload.svg
    assets/icons/icon_info.svg
    assets/icons/icon_open_in_new.svg
    assets/icons/icon_plus.svg
    assets/icons/icon_search.svg
    assets/icons/icon_steam.svg
    assets/icons/icon_thumb_down.svg
    assets/icons/icon_thumb_up.svg
    assets/images/mask_workshop.png
    assets/images/steam_default_avatar.png
    assets/images/steam_offline.png)

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

qt_add_library(${PROJECT_NAME} STATIC ${RESOURCES})
# Needed by the automatic generated target missing includes
# https://github.com/qt/qtdeclarative/blob/7a7064e14f094e843e1ee832cc927e86f887621a/src/qml/Qt6QmlMacros.cmake#L2042
target_include_directories(${PROJECT_NAME} PUBLIC src/)
target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC Qt6::Core
           Qt6::Quick
           ${STEAM_LIB}
           ScreenPlayUtil
           SteamSDK)

qt_add_qml_module(
    ${PROJECT_NAME}
    URI
    ${PROJECT_NAME}
    OUTPUT_DIRECTORY
    ${SCREENPLAY_QML_MODULES_PATH}/${PROJECT_NAME}
    RESOURCE_PREFIX
    /qml
    VERSION
    1.0
    QML_FILES
    ${QML}
    SOURCES
    ${SOURCES}
    ${HEADER}
    RESOURCES
    ${RESOURCES})

if(${SCREENPLAY_STEAM})
    if(APPLE)
        set(MACOS_FRAMEWORKS_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ScreenPlay.app/Contents/MacOS/)
        file(MAKE_DIRECTORY ${MACOS_FRAMEWORKS_DIR})
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/steam_appid.txt ${MACOS_FRAMEWORKS_DIR} COPYONLY)
        configure_file(${STEAM_BIN} ${MACOS_FRAMEWORKS_DIR} COPYONLY)
    endif()

    if(${SCREENPLAY_TESTS})
        qt_add_executable(tst_ScreenPlayWorkshop src/TestMain.cpp)
        target_link_libraries(
            tst_ScreenPlayWorkshop
            PRIVATE Qt6::Quick
                    Qt6::QuickControls2
                    ${PROJECT_NAME}plugin
                    ScreenPlayUtilplugin
                    SteamSDK)

    if(APPLE)
        set_target_properties(tst_ScreenPlayWorkshop PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/ScreenPlay.app/Contents/MacOS/")
    endif()
    endif()
endif()
