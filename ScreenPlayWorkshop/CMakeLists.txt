project(
    ScreenPlayWorkshop
    VERSION 0.1
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(
    Qt6
    COMPONENTS Core Quick
    REQUIRED)

set(QML
    src/TestMain.qml
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
    qml/upload/PopupSteamWorkshopAgreement.qml
    qml/upload/UploadProject.qml
    qml/upload/UploadProjectBigItem.qml
    qml/upload/UploadProjectItem.qml
    qml/Workshop.qml
    qml/WorkshopInstalled.qml
    qml/WorkshopItem.qml)

set(SOURCES
    src/installedlistmodel.cpp
    src/steamaccount.cpp
    src/steamapiwrapper.cpp
    src/steamqmlimageprovider.cpp
    src/steamworkshop.cpp
    src/steamworkshopitem.cpp
    src/steamworkshoplistmodel.cpp
    src/workshop.cpp)

set(HEADER
    src/installedlistmodel.h
    src/steamaccount.h
    src/steamapiwrapper.h
    src/steamqmlimageprovider.h
    src/steamworkshop.h
    src/steamworkshopitem.h
    src/steamworkshoplistmodel.h
    src/uploadlistmodel.h
    src/workshop.h
    src/workshopitem.h)

set(RESOURCES
    assets/icons/icon_download.svg
    assets/icons/icon_open_in_new.svg
    assets/icons/icon_plus.svg
    assets/icons/icon_steam.svg
    assets/icons/icon_thumb_down.svg
    assets/icons/icon_thumb_up.svg
    assets/icons/icon_arrow_left.svg
    assets/icons/icon_arrow_right.svg
    assets/icons/icon_search.svg
    assets/icons/icon_info.svg
    assets/images/steam_default_avatar.png
    assets/images/steam_offline.png
    assets/images/mask_workshop.png)

set(STEAM_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/SteamSDK/redistributable_bin/")

if(WIN32)
    set(WORKSHOP_PLUGIN_DIR ${SCREENPLAY_QML_MODULES_PATH}/${PROJECT_NAME})
    set(STEAM_LIB "${STEAM_LIB_PATH}/win64/steam_api64.lib")
    set(STEAM_BIN "${STEAM_LIB_PATH}/win64/steam_api64.dll")
elseif(APPLE)
    set(WORKSHOP_PLUGIN_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ScreenPlay.app/Contents/MacOS/ScreenPlay/qml/${PROJECT_NAME})
    set(MACOS_FRAMEWORKS_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ScreenPlay.app/Contents/Frameworks/)
    set(STEAM_LIB "${STEAM_LIB_PATH}/osx/libsteam_api.dylib")
    set(STEAM_BIN ${STEAM_LIB})
elseif(UNIX)
    set(WORKSHOP_PLUGIN_DIR ${SCREENPLAY_QML_MODULES_PATH}/${PROJECT_NAME})
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
           SteamSDK
           SteamSDKQtEnums)

qt_add_qml_module(
    ${PROJECT_NAME}
    URI
    ${PROJECT_NAME}
    OUTPUT_DIRECTORY
    ${WORKSHOP_PLUGIN_DIR}
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
        file(MAKE_DIRECTORY ${WORKSHOP_PLUGIN_DIR})
        file(MAKE_DIRECTORY ${MACOS_FRAMEWORKS_DIR})

        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/steam_appid.txt ${CMAKE_BINARY_DIR}/bin/ScreenPlay.app/Contents/MacOS/ COPYONLY)
        configure_file(${STEAM_BIN} ${WORKSHOP_PLUGIN_DIR} COPYONLY)

        set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY
                                                         ${CMAKE_BINARY_DIR}/bin/ScreenPlay.app/Contents/MacOS/Workshop)
    else()
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/steam_appid.txt ${CMAKE_BINARY_DIR}/bin/steam_appid.txt COPYONLY)
        configure_file(${STEAM_BIN} ${CMAKE_BINARY_DIR}/bin/ COPYONLY)
    endif()

    if(${SCREENPLAY_TESTS})
        qt_add_executable(tst_ScreenPlayWorkshop src/TestMain.cpp)
        target_link_libraries(
            tst_ScreenPlayWorkshop
            PRIVATE Qt6::Quick
                    ${PROJECT_NAME}plugin
                    ScreenPlayUtilplugin
                    SteamSDK
                    SteamSDKQtEnums)
        target_compile_definitions(tst_ScreenPlayWorkshop PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
    endif()
endif()
