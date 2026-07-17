# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
project(ScreenPlayCore LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

find_package(fmt CONFIG REQUIRED)

# Needed on macos
find_package(Threads REQUIRED)
find_package(LibArchive REQUIRED)
find_package(
    Qt6
    COMPONENTS Core
               Quick
               Gui
               ShaderTools
    REQUIRED)

qt_standard_project_setup(REQUIRES 6.8)

set(QML
    # cmake-format: sort
    qml/CloseIcon.qml
    qml/ColorImage.qml
    qml/Dialog.qml
    qml/Dialogs/CriticalError.qml
    qml/Dialogs/MonitorConfiguration.qml
    qml/Dialogs/SteamNotAvailable.qml
    qml/ErrorPopup.qml
    qml/FileDropAnimation.qml
    qml/FileSelector.qml
    qml/FrameStatsOverlay.qml
    qml/FireworksEffect.qml
    qml/Grow.qml
    qml/GrowIconLink.qml
    qml/Headline.qml
    qml/HeadlineSection.qml
    qml/ImageBlurContainer.qml
    qml/ImageSelector.qml
    qml/InstantPopup.js
    qml/LabelSlider.qml
    qml/LicenseSelector.qml
    qml/MaterialGridView.qml
    qml/MaterialListView.qml
    qml/MaterialSnackBar.qml
    qml/ModalBackgroundBlur.qml
    qml/MouseHoverBlocker.qml
    qml/RainbowGradient.qml
    qml/RippleEffect.qml
    qml/Shake.qml
    qml/SPPopup.qml
    qml/Tag.qml
    qml/TagSelector.qml
    qml/TextField.qml)

set(SOURCES
    # cmake-format: sort
    src/archivereader.cpp
    src/contenttypes.cpp
    src/exitcodes.cpp
    src/framestats.cpp
    src/godothandler.cpp
    src/licensemanager.cpp
    src/logginghandler.cpp
    src/processmanager.cpp
    src/projectfile.cpp
    src/sparchive.cpp
    src/steamenumsgenerated.cpp
    src/util.cpp)

set(HEADER
    # cmake-format: sort
    inc/public/ScreenPlayCore/archivereader.h
    inc/public/ScreenPlayCore/contenttypes.h
    inc/public/ScreenPlayCore/exitcodes.h
    inc/public/ScreenPlayCore/framestats.h
    inc/public/ScreenPlayCore/globalenums.h
    inc/public/ScreenPlayCore/godothandler.h
    inc/public/ScreenPlayCore/HelpersCommon.h
    inc/public/ScreenPlayCore/licensemanager.h
    inc/public/ScreenPlayCore/ListPropertyHelper.h
    inc/public/ScreenPlayCore/logginghandler.h
    inc/public/ScreenPlayCore/processmanager.h
    inc/public/ScreenPlayCore/projectfile.h
    inc/public/ScreenPlayCore/PropertyHelpers.h
    inc/public/ScreenPlayCore/sparchive.h
    inc/public/ScreenPlayCore/steamenumsgenerated.h
    inc/public/ScreenPlayCore/util.h)

set(SHADER # cmake-format: sort
           qml/shaders/color_image.frag)

if(APPLE)
    list(APPEND SOURCES src/macutils.mm)
    list(APPEND HEADER inc/public/ScreenPlayCore/macutils.h)
endif()

if(WIN32)
    add_library(ScreenPlayCoreWindowsExtrasLib STATIC inc/public/ScreenPlayCore/windowsutils.h src/windowsutils.cpp)
    target_link_libraries(ScreenPlayCoreWindowsExtrasLib PUBLIC shcore.lib)
    target_include_directories(ScreenPlayCoreWindowsExtrasLib PUBLIC inc/public/)
endif()

# IPC frame reassembly shared by every endpoint of the wallpaper/widget
# socket. Qt-free on purpose: it is also compiled into the Godot GDExtension
# (ScreenPlayWallpaper/Godot/GDExtention), which cannot link Qt.
add_library(ScreenPlayCoreIpcLib STATIC inc/public/ScreenPlayCore/ipcframebuffer.h src/ipcframebuffer.cpp)
target_compile_features(ScreenPlayCoreIpcLib PUBLIC cxx_std_17)
target_include_directories(ScreenPlayCoreIpcLib PUBLIC inc/public/)
set_target_properties(ScreenPlayCoreIpcLib PROPERTIES POSITION_INDEPENDENT_CODE ON)

set(RESOURCES
    # cmake-format: sort
    assets/icons/attach_file.svg
    assets/icons/brand_bluesky.svg
    assets/icons/brand_github.svg
    assets/icons/brand_gitlab.svg
    assets/icons/brand_reddit.svg
    assets/icons/brand_twitch.svg
    assets/icons/description.svg
    assets/icons/exclamation-triangle-solid.svg
    assets/icons/folder.svg
    assets/icons/font-awsome/close.svg
    assets/icons/font-awsome/frown-o.svg
    assets/icons/font-awsome/lock-solid.svg
    assets/icons/font-awsome/patreon-brands.svg
    assets/icons/font-awsome/rotate-right-solid.svg
    assets/icons/icon_arrow_left.svg
    assets/icons/icon_arrow_right.svg
    assets/icons/icon_build.svg
    assets/icons/icon_cached.svg
    assets/icons/icon_cake.svg
    assets/icons/icon_cancel_presentation.svg
    assets/icons/icon_close.svg
    assets/icons/icon_close_black_24dp.svg
    assets/icons/icon_code.svg
    assets/icons/icon_community.svg
    assets/icons/icon_contains_audio.svg
    assets/icons/icon_delete.svg
    assets/icons/icon_document.svg
    assets/icons/icon_done.svg
    assets/icons/icon_download.svg
    assets/icons/icon_edit.svg
    assets/icons/icon_emptyWidget.svg
    assets/icons/icon_folder_open.svg
    assets/icons/icon_forum.svg
    assets/icons/icon_help_center.svg
    assets/icons/icon_import_export_.svg
    assets/icons/icon_indicator_down.svg
    assets/icons/icon_info.svg
    assets/icons/icon_installed.svg
    assets/icons/icon_launch.svg
    assets/icons/icon_minimize.svg
    assets/icons/icon_movie.svg
    assets/icons/icon_new_releases.svg
    assets/icons/icon_open_in_new.svg
    assets/icons/icon_open_in_new_black.svg
    assets/icons/icon_pause.svg
    assets/icons/icon_people.svg
    assets/icons/icon_play.svg
    assets/icons/icon_plus.svg
    assets/icons/icon_report_problem.svg
    assets/icons/icon_schedule.svg
    assets/icons/icon_search.svg
    assets/icons/icon_settings.svg
    assets/icons/icon_share.svg
    assets/icons/icon_single_image.svg
    assets/icons/icon_sort-down-solid.svg
    assets/icons/icon_sort-up-solid.svg
    assets/icons/icon_steam.svg
    assets/icons/icon_supervisor_account.svg
    assets/icons/icon_thumb_down.svg
    assets/icons/icon_thumb_up.svg
    assets/icons/icon_upload.svg
    assets/icons/icon_video_settings.svg
    assets/icons/icon_volume.svg
    assets/icons/icon_volume_mute.svg
    assets/icons/icon_volume_up.svg
    assets/icons/icon_widgets.svg
    assets/icons/icon_window.svg
    assets/icons/item_banner_new.svg
    assets/icons/monitor_setup.svg)

qt_add_library(ScreenPlayCoreLib STATIC)
target_compile_features(ScreenPlayCoreLib PRIVATE cxx_std_23)

qt_add_qml_module(
    ScreenPlayCoreLib
    URI
    ScreenPlayCore
    VERSION
    1.0
    QML_FILES
    ${QML}
    RESOURCES
    ${RESOURCES}
    SOURCES
    ${SOURCES}
    ${HEADER})

# https://doc.qt.io/qt-6/qtshadertools-build.html
qt6_add_shaders(
    ScreenPlayCoreLibplugin
    "ScreenPlayCoreLib_shaders"
    PREFIX
    "/"
    BATCHABLE
    PRECOMPILE
    FILES
    ${SHADER})

target_include_directories(
    ScreenPlayCoreLib
    PUBLIC inc/public/
    PRIVATE src/)

# Note making this public is so thatScreenPlayCore_qmltyperegistrations.cpp can find the #include <contenttypes.h> include
target_include_directories(ScreenPlayCoreLib PUBLIC inc/public/ScreenPlayCore)
target_include_directories(ScreenPlayCoreLibplugin PUBLIC inc/public/ScreenPlayCore)

target_link_libraries(
    ScreenPlayCoreLib
    PUBLIC Qt6::Core
           Qt6::Quick
           Qt6::Gui
           QCoro6::Core
           QCoro6::Qml
           ScreenPlayCoreIpcLib
           fmt::fmt-header-only
           LibArchive::LibArchive
           QArchive)

if(WIN32)
    # Used for query windows monitor data
    target_link_libraries(ScreenPlayCoreLib PUBLIC shcore.lib)
endif()

# ##### USE CMAKE VARIABLES IN CODE #####
include(GenerateCMakeVariableHeader)
generate_cmake_variable_header(ScreenPlayCoreLib)
