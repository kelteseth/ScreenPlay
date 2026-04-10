# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
project(
    ScreenPlayWorkshop
    VERSION 0.1
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(GenerateCMakeVariableHeader)

find_package(LibArchive REQUIRED)
find_package(
    Qt6
    COMPONENTS Core
               Quick
               QuickControls2
               Widgets
               WebSockets
               Svg
               Xml
    REQUIRED)

qt_standard_project_setup(REQUIRES 6.8)

set(QML
    # cmake-format: sort
    qml/Background.qml
    qml/Forum.qml
    qml/Navigation.qml
    qml/PopupOffline.qml
    qml/Sidebar.qml
    qml/SteamProfile.qml
    qml/SteamProfileWorkshopItem.qml
    qml/SteamWorkshop.qml
    qml/SteamWorkshopView.qml
    qml/TestMain.qml
    qml/upload/PopupSteamWorkshopAgreement.qml
    qml/upload/SteamUploadView.qml
    qml/upload/UploadProjectBigItem.qml
    qml/upload/UploadProjectItem.qml
    qml/Workshop.qml
    qml/WorkshopItem.qml)

set(SOURCES
    # cmake-format: sort
    src/installedlistmodel.cpp
    src/screenplayworkshop.cpp
    src/steamaccount.cpp
    src/steamqmlimageprovider.cpp
    src/steamworkshop.cpp
    src/steamworkshopitem.cpp
    src/steamworkshopitemops.cpp
    src/steamworkshoplistmodel.cpp
    src/steamworkshopprofile.cpp
    src/steamworkshopsearch.cpp)

set(HEADER
    # cmake-format: sort
    src/installedlistfilter.h
    src/installedlistmodel.h
    src/screenplayworkshop.h
    src/steamaccount.h
    src/steamasynccall.h
    src/steamqmlimageprovider.h
    src/steamtagarray.h
    src/steamworkshop.h
    src/steamworkshopitem.h
    src/steamworkshopitemops.h
    src/steamworkshoplistmodel.h
    src/steamworkshopprofile.h
    src/steamworkshopsearch.h
    src/ugcquerybuilder.h
    src/uploadlistmodel.h
    src/workshopitem.h
    src/workshopitemdetail.h)

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
elseif(APPLE)
    set(STEAM_LIB "${STEAM_LIB_PATH}/osx/libsteam_api.dylib")
elseif(UNIX)
    set(STEAM_LIB "${STEAM_LIB_PATH}/linux64/libsteam_api.so")
endif()

qt_add_library(ScreenPlayWorkshopLib STATIC)
target_compile_features(ScreenPlayWorkshopLib PRIVATE cxx_std_23)

# Needed by the automatic generated target missing includes
# https://github.com/qt/qtdeclarative/blob/7a7064e14f094e843e1ee832cc927e86f887621a/src/qml/Qt6QmlMacros.cmake#L2042
target_include_directories(ScreenPlayWorkshopLib PUBLIC src/)

target_link_libraries(
    ScreenPlayWorkshopLib
    PUBLIC Qt6::Core
           Qt6::Quick
           ${STEAM_LIB}
           ScreenPlayCoreLib
           ScreenPlayCoreLibplugin
           SteamSDK)

qt_add_qml_module(
    ScreenPlayWorkshopLib
    URI
    ScreenPlayWorkshop
    VERSION
    1.0
    QML_FILES
    ${QML}
    SOURCES
    ${SOURCES}
    ${HEADER}
    RESOURCES
    ${RESOURCES})

# Add qtquickcontrols2.conf at the root resource path so Qt can find it This is needed for test executables that link to this library
qt_add_resources(
    ScreenPlayWorkshopLib
    "qtquickcontrols2"
    PREFIX
    "/"
    FILES
    qtquickcontrols2.conf)

if(${SCREENPLAY_STEAM})
    # Copy Steam runtime files to all output directories that need them
    set(_steam_copy_dirs ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

    # Test executables in the ScreenPlay subproject have a different output directory
    if(${SCREENPLAY_TESTS})
        list(APPEND _steam_copy_dirs "${CMAKE_BINARY_DIR}/ScreenPlay")
    endif()

    list(REMOVE_DUPLICATES _steam_copy_dirs)

    foreach(_dir ${_steam_copy_dirs})
        file(MAKE_DIRECTORY "${_dir}")
        file(COPY_FILE "${STEAM_BINARY_PATH}" "${_dir}/${STEAM_BINARY}" ONLY_IF_DIFFERENT)
        file(COPY_FILE "${CMAKE_CURRENT_SOURCE_DIR}/steam_appid.txt" "${_dir}/steam_appid.txt" ONLY_IF_DIFFERENT)
    endforeach()

    if(APPLE)
        install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/steam_appid.txt" DESTINATION "ScreenPlay.app/Contents/MacOS/")
        install(FILES "${STEAM_BINARY_PATH}" DESTINATION "ScreenPlay.app/Contents/MacOS/")
    else()
        install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/steam_appid.txt" DESTINATION "${CMAKE_INSTALL_BINDIR}")
        install(FILES "${STEAM_BINARY_PATH}" DESTINATION "${CMAKE_INSTALL_BINDIR}")
    endif()

    if(${SCREENPLAY_TESTS})
        qt_add_executable(tst_ScreenPlayWorkshop src/tst_ScreenPlayWorkshop.cpp)
        target_link_libraries(
            tst_ScreenPlayWorkshop
            PRIVATE Qt6::Quick
                    Qt6::QuickControls2
                    ScreenPlayLib
                    ScreenPlayLibplugin
                    ScreenPlayWorkshopLib
                    ScreenPlayWorkshopLibplugin
                    ScreenPlayCoreLib
                    ScreenPlayCoreLibplugin)

        qt_add_executable(tst_WorkshopUpload tests/tst_workshop_upload.cpp)
        target_link_libraries(
            tst_WorkshopUpload
            PRIVATE Qt6::Quick
                    Qt6::QuickControls2
                    Qt6::Test
                    ScreenPlayWorkshopLib
                    ScreenPlayWorkshopLibplugin
                    ScreenPlayCoreLib
                    ScreenPlayCoreLibplugin)

        # Generate CMakeVariables.h for the workshop upload test
        generate_cmake_variable_header(tst_WorkshopUpload)
        add_dependencies(tst_WorkshopUpload ScreenPlayWorkshopLib)

        if(APPLE)
            set_target_properties(tst_ScreenPlayWorkshop
                                  PROPERTIES RUNTIME_OUTPUT_DIRECTORY"${CMAKE_BINARY_DIR}/bin/ScreenPlay.app/Contents/MacOS/")
            set_target_properties(tst_WorkshopUpload
                                  PROPERTIES RUNTIME_OUTPUT_DIRECTORY"${CMAKE_BINARY_DIR}/bin/ScreenPlay.app/Contents/MacOS/")
        endif()
    endif()
endif()
