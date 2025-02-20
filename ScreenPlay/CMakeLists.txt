# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
project(ScreenPlay LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

# ##### USE CMAKE VARIABLES IN CODE #####
include(GenerateCMakeVariableHeader)

set(SOURCES
    # cmake-format: sort
    src/app.cpp
    src/create.cpp
    src/createimportvideo.cpp
    src/globalvariables.cpp
    src/installedlistfilter.cpp
    src/installedlistmodel.cpp
    src/monitorlistmodel.cpp
    src/profilelistmodel.cpp
    src/projectsettingslistmodel.cpp
    src/screenplaymanager.cpp
    src/screenplaytimelinemanager.cpp
    src/screenplaywallpaper.cpp
    src/screenplaywidget.cpp
    src/sdkconnection.cpp
    src/settings.cpp
    src/wallpaperdata.cpp
    src/wallpapertimelinesection.cpp
    src/wizards.cpp)

set(HEADER
    # cmake-format: sort
    inc/public/ScreenPlay/app.h
    inc/public/ScreenPlay/create.h
    inc/public/ScreenPlay/createimportstates.h
    inc/public/ScreenPlay/createimportvideo.h
    inc/public/ScreenPlay/globalvariables.h
    inc/public/ScreenPlay/installedlistfilter.h
    inc/public/ScreenPlay/installedlistmodel.h
    inc/public/ScreenPlay/monitorlistmodel.h
    inc/public/ScreenPlay/profile.h
    inc/public/ScreenPlay/profilelistmodel.h
    inc/public/ScreenPlay/projectsettingslistmodel.h
    inc/public/ScreenPlay/screenplaymanager.h
    inc/public/ScreenPlay/screenplaytimelinemanager.h
    inc/public/ScreenPlay/screenplaywallpaper.h
    inc/public/ScreenPlay/screenplaywidget.h
    inc/public/ScreenPlay/sdkconnection.h
    inc/public/ScreenPlay/settings.h
    inc/public/ScreenPlay/wallpaperdata.h
    inc/public/ScreenPlay/wallpapertimelinesection.h
    inc/public/ScreenPlay/wizards.h)

set(QML
    # cmake-format: sort
    qml/Community/CommunityNavItem.qml
    qml/Community/CommunityView.qml
    qml/Community/XMLNewsfeed.qml
    qml/Components/LineHandle.qml
    qml/Components/LineIndicator.qml
    qml/Components/ScreenPlayProPopup.qml
    qml/Components/Timeline.qml
    qml/Components/TimelineSection.qml
    qml/Components/TrayIcon.qml
    qml/ContentSettings/ContentSettingsView.qml
    qml/ContentSettings/DefaultVideoControls.qml
    qml/ContentSettings/MonitorSelection.qml
    qml/ContentSettings/MonitorSelectionItem.qml
    qml/ContentSettings/MonitorsProjectSettingItem.qml
    qml/ContentSettings/SaveNotification.qml
    qml/Create/CreateSidebar.qml
    qml/Create/CreateView.qml
    qml/Create/StartInfo.qml
    qml/Create/StartInfoLinkImage.qml
    qml/Create/Wizard.qml
    qml/Create/Wizards/GifWallpaper.qml
    qml/Create/Wizards/GodotWallpaper.qml
    qml/Create/Wizards/HTMLWallpaper.qml
    qml/Create/Wizards/HTMLWidget.qml
    qml/Create/Wizards/ImportVideoAndConvert/CreateWallpaper.qml
    qml/Create/Wizards/ImportVideoAndConvert/CreateWallpaperFileSelect.qml
    qml/Create/Wizards/ImportVideoAndConvert/CreateWallpaperResult.qml
    qml/Create/Wizards/ImportVideoAndConvert/CreateWallpaperSettings.qml
    qml/Create/Wizards/ImportVideoAndConvert/CreateWallpaperVideoImportConvert.qml
    qml/Create/Wizards/QMLWallpaper.qml
    qml/Create/Wizards/QMLWidget.qml
    qml/Create/Wizards/WebsiteWallpaper.qml
    qml/Create/Wizards/WizardPage.qml
    qml/Installed/InstalledDrawer.qml
    qml/Installed/InstalledItem.qml
    qml/Installed/InstalledItemImage.qml
    qml/Installed/InstalledNavigation.qml
    qml/Installed/InstalledView.qml
    qml/Installed/InstalledWelcomeScreen.qml
    qml/Navigation/ExitPopup.qml
    qml/Navigation/Navigation.qml
    qml/ScreenPlayMain.qml
    qml/Settings/SettingBool.qml
    qml/Settings/SettingsButton.qml
    qml/Settings/SettingsComboBox.qml
    qml/Settings/SettingsExpander.qml
    qml/Settings/SettingsHeader.qml
    qml/Settings/SettingsHorizontalSeperator.qml
    qml/Settings/SettingsPage.qml
    qml/Settings/SettingsView.qml
    qml/Workshop/WorkshopView.qml)

set(TS_FILES
    # cmake-format: sort
    translations/ScreenPlay_.ts
    translations/ScreenPlay_de_DE.ts
    translations/ScreenPlay_es_ES.ts
    translations/ScreenPlay_fr_FR.ts
    translations/ScreenPlay_it_IT.ts
    translations/ScreenPlay_ko_KR.ts
    translations/ScreenPlay_nl_NL.ts
    translations/ScreenPlay_pl_PL.ts
    translations/ScreenPlay_pt_BR.ts
    translations/ScreenPlay_ru_RU.ts
    translations/ScreenPlay_tr_TR.ts
    translations/ScreenPlay_vi_VN.ts
    translations/ScreenPlay_zh_CN.ts)

set(RESOURCES
    # cmake-format: sort
    "legal/Font Awesome Free License.txt"
    "legal/Qt LGPLv3.txt"
    assets/icons/app.ico
    assets/icons/brand_github.svg
    assets/icons/brand_gitlab.svg
    assets/icons/brand_reddit.svg
    assets/icons/brand_twitch.svg
    assets/icons/brand_bluesky.svg
    assets/icons/exclamation-triangle-solid.svg
    assets/icons/font-awsome/close.svg
    assets/icons/font-awsome/frown-o.svg
    assets/icons/font-awsome/lock-solid.svg
    assets/icons/font-awsome/patreon-brands.svg
    assets/icons/font-awsome/rotate-right-solid.svg
    assets/icons/icon_arrow_left.svg
    assets/icons/icon_arrow_right.svg
    assets/icons/icon_build.svg
    assets/icons/icon_cake.afdesign
    assets/icons/icon_cake.svg
    assets/icons/icon_cancel_presentation.svg
    assets/icons/icon_close.svg
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
    assets/icons/monitor_setup.svg
    assets/icons/steam_default_avatar.png
    assets/images/first_startup_mask.png
    assets/images/Intro.png
    assets/images/Intro_PC.png
    assets/images/Intro_shine.png
    assets/images/mask_round.svg
    assets/images/mask_workshop.png
    assets/images/missingPreview.png
    assets/images/noisy-texture-3.png
    assets/images/noisy-texture.png
    assets/images/pro_version.png
    assets/images/rocket_3d.png
    assets/images/scale_window_indicator.png
    assets/images/steam_offline.png
    assets/images/timeline_mask.png
    assets/images/trayIcon_osx.png
    assets/images/trayIcon_windows.png
    assets/images/Window.svg
    assets/licenses/Apache2.txt
    assets/licenses/OFL.txt
    assets/macos/app.screenplay.plist
    assets/startinfo/blender.png
    assets/startinfo/flaticon.png
    assets/startinfo/forums.png
    assets/startinfo/freesound.png
    assets/startinfo/gimp.png
    assets/startinfo/git_extentions.png
    assets/startinfo/gitlab.png
    assets/startinfo/godot.png
    assets/startinfo/handbreak.png
    assets/startinfo/inkscape.png
    assets/startinfo/kdeenlive.png
    assets/startinfo/krita.png
    assets/startinfo/obs.png
    assets/startinfo/qml_online.png
    assets/startinfo/reddit.png
    assets/startinfo/shadertoy.png
    assets/startinfo/sharex.png
    assets/startinfo/unsplash.png
    assets/startinfo/vscode.png
    assets/wizards/example_html.png
    assets/wizards/example_qml.png
    assets/wizards/License_All_Rights_Reserved_1.0.txt
    assets/wizards/License_Apache_2.0.txt
    assets/wizards/License_CC0_1.0.txt
    assets/wizards/License_CC_Attribution-NonCommercial-ShareAlike_4.0.txt
    assets/wizards/License_CC_Attribution-ShareAlike_4.0.txt
    assets/wizards/License_CC_Attribution_4.0.txt
    assets/wizards/License_GPL_3.0.txt
    legal/DataProtection.txt
    legal/gpl-3.0.txt
    legal/lgpl-2.1.txt
    legal/OFL.txt
    legal/OpenSSL.txt
    profiles.json
    qml/Create/WizardsFiles/Godot_v5/export_presets.cfg
    qml/Create/WizardsFiles/Godot_v5/project.godot
    qml/Create/WizardsFiles/Godot_v5/spinner.gd
    qml/Create/WizardsFiles/Godot_v5/wallpaper.tscn
    qml/Create/WizardsFiles/HTMLWallpaperMain.html
    qml/Create/WizardsFiles/HTMLWidgetMain.html
    qml/Create/WizardsFiles/QmlProject.qmlproject
    qml/Create/WizardsFiles/QMLWallpaperMain.qml
    qml/Create/WizardsFiles/QMLWidgetMain.qml
    qtquickcontrols2.conf)

# Needed on macos
find_package(Threads REQUIRED)
find_package(LibArchive REQUIRED)

# Make sentry win only for now because it is constantly buggy on osx
if(WIN32)
    # CURL must be included before sentry because sentry needs the module and does not include it itself on macos...
    find_package(CURL CONFIG REQUIRED)
    find_package(sentry CONFIG REQUIRED)
endif()

find_package(
    Qt6
    COMPONENTS Core
               Quick
               QuickControls2
               Gui
               Widgets
               WebSockets
               Svg
               Xml
               LinguistTools
               Test)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_library(ScreenPlayLib STATIC)

target_compile_features(ScreenPlayLib PRIVATE cxx_std_23)

# Note making this public is so that *_qmltyperegistrations.cpp can find the needed include
target_include_directories(
    ScreenPlayLib
    PUBLIC inc/public/ inc/public/ScreenPlay
    PRIVATE src/)

# ScreenPlayLib is our qml module needed for compiling of all classes and most importanly for QML_ELEMENT. So our app is mostly a module
# that then link to ScreenPlay executable.
qt_add_qml_module(
    ScreenPlayLib
    URI
    ScreenPlay
    VERSION
    1.0
    QML_FILES
    ${QML}
    SOURCES
    ${SOURCES}
    ${HEADER}
    RESOURCES
    ${RESOURCES})

target_link_libraries(
    ScreenPlayLib
    PUBLIC ScreenPlaySDKLib
           LibArchive::LibArchive
           ScreenPlayCoreLib
           ScreenPlayCoreLibplugin
           QArchive
           Threads::Threads
           QCoro6::Qml
           Qt6::Quick
           Qt6::Gui
           Qt6::Widgets
           Qt6::Core
           Qt6::WebSockets
           Qt6::Svg
           Qt6::QuickControls2
           Qt6::Xml)

if(${SCREENPLAY_STEAM})
    target_link_libraries(ScreenPlayLib PUBLIC ScreenPlayWorkshopLibplugin ScreenPlayWorkshopLib)
endif()

qt_add_executable(ScreenPlayApp main.cpp)

target_link_libraries(ScreenPlayApp PRIVATE ScreenPlayLib ScreenPlayLibplugin)

if(${SCREENPLAY_TESTS})
    qt_add_executable(tst_ScreenPlay tests/tst_main.cpp)
    target_link_libraries(tst_ScreenPlay PRIVATE ScreenPlayLib ScreenPlayLibplugin Qt6::Test)
    generate_cmake_variable_header(tst_ScreenPlay)

    if(${SCREENPLAY_STEAM})
        target_link_libraries(tst_ScreenPlay PUBLIC ScreenPlayWorkshopplugin ScreenPlayWorkshop)
    endif()
endif()

if(WIN32)
    include(CopyRecursive)
    set(FONTS_OUT_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/fonts)
    file(MAKE_DIRECTORY ${FONTS_OUT_DIR})
    copy_recursive(${CMAKE_CURRENT_SOURCE_DIR}/assets/fonts ${FONTS_OUT_DIR} "*.ttf")
    copy_recursive(${CMAKE_CURRENT_SOURCE_DIR}/assets/fonts ${FONTS_OUT_DIR} "*.otf")
    set(RESOURCES_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets")
    file(MAKE_DIRECTORY ${RESOURCES_DIR})
endif()

if(UNIX AND NOT APPLE)
    set(RESOURCES_DIR "${CMAKE_BINARY_DIR}/ScreenPlay/assets")
    file(MAKE_DIRECTORY ${RESOURCES_DIR})
endif()

if(WIN32)
    target_link_libraries(ScreenPlayLib PUBLIC CURL::libcurl sentry::sentry ScreenPlayWallpaperLib)

    # Icon
    target_sources(ScreenPlayLib PRIVATE ScreenPlay.rc)

    # Needed for the installscript.qs Windows entry
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/assets/icons/app.ico ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} COPYONLY)

    # Disable console window on Windows
    # https://stackoverflow.com/questions/8249028/how-do-i-keep-my-qt-c-program-from-opening-a-console-in-windows
    set_property(TARGET ScreenPlayApp PROPERTY WIN32_EXECUTABLE true)

    # Copy ffmpeg. If the ffmpeg files are missing, start the install_dependencies_XXX for your system!
    file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/ffmpeg/*")

    foreach(filename ${files})
        configure_file(${filename} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} COPYONLY)
    endforeach()

    configure_file(${VCPKG_INSTALLED_PATH}/tools/sentry-native/crashpad_handler.exe ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ COPYONLY)
    configure_file(${VCPKG_INSTALLED_PATH}/tools/sentry-native/zlib1.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ COPYONLY)

    configure_file(${VCPKG_BIN_PATH}/libssl-3-x64.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ COPYONLY)
    configure_file(${VCPKG_BIN_PATH}/libcrypto-3-x64.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ COPYONLY)
endif()

if(APPLE)
    # Set the path to the icon file
    set(APP_ICON_MACOSX ${CMAKE_CURRENT_SOURCE_DIR}/assets/icons/ScreenPlay.icns)

    set(RESOURCES_DIR "$<TARGET_BUNDLE_DIR:ScreenPlayApp>/Contents/Resources")
    file(MAKE_DIRECTORY ${RESOURCES_DIR})

    # Specify the name of the icon file for the bundle
    set(MACOSX_BUNDLE_ICON_FILE ScreenPlay.icns) # Only the file name

    # Fore some reason this only works with maually copy configure_file(${APP_ICON_MACOSX} "${RESOURCES_DIR}/ScreenPlay.icns" COPYONLY)

    # Creates a ScreenPlay.app Note: Do NOT set MACOSX_BUNDLE TRUE for additional targets
    set_target_properties(
        ScreenPlayApp
        PROPERTIES OUTPUT_NAME "ScreenPlay" # This controls the .app name
                   MACOSX_BUNDLE TRUE
                   MACOSX_BUNDLE_BUNDLE_NAME "ScreenPlay" # This controls the bundle name
                   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
                   MACOSX_RPATH TRUE
                   MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist
                   MACOSX_FRAMEWORK_IDENTIFIER app.screenplay
                   XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS "@loader_path/Libraries"
                   RESOURCE "${RESOURCE_FILES};${APP_ICON_MACOSX}"
                   XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME TRUE
                   XCODE_ATTRIBUTE_EXECUTABLE_NAME "ScreenPlay") # This controls the executable name inside the bundle

    add_custom_command(
        TARGET ScreenPlayApp
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/ffmpeg/ffmpeg"
                "$<TARGET_BUNDLE_DIR:ScreenPlayApp>/Contents/MacOS/")

    add_custom_command(
        TARGET ScreenPlayApp
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/ffmpeg/ffprobe"
                "$<TARGET_BUNDLE_DIR:ScreenPlayApp>/Contents/MacOS/")

    # fonts
    include(CopyRecursive)
    set(FONTS_OUT_DIR "$<TARGET_BUNDLE_DIR:ScreenPlayApp>/Contents/Resources/fonts")
    file(MAKE_DIRECTORY ${FONTS_OUT_DIR})
    copy_recursive(${CMAKE_CURRENT_SOURCE_DIR}/assets/fonts ${FONTS_OUT_DIR} "*.ttf")
    copy_recursive(${CMAKE_CURRENT_SOURCE_DIR}/assets/fonts ${FONTS_OUT_DIR} "*.otf")
endif()

# Must be called here, because we need to change the OUTPUT_LOCATION for macos
qt_add_lrelease(
    TS_FILES
    ${TS_FILES}
    LRELEASE_TARGET
    screenplay_translations
    QM_FILES_OUTPUT_VARIABLE
    qm_files)
if(APPLE)
    add_dependencies(ScreenPlayApp screenplay_translations)
else()
    # .qm translations
    set(QM_OUT_DIR "${RESOURCES_DIR}/translations")
    file(MAKE_DIRECTORY ${QM_OUT_DIR})
    set_source_files_properties(${TS_FILES} PROPERTIES OUTPUT_LOCATION ${QM_OUT_DIR})
endif()

if(${SCREENPLAY_STEAM})
    if(APPLE)
        # Add post-build commands for Steam integration
        add_custom_command(
            TARGET ScreenPlayApp
            POST_BUILD
            # Copy Steam API library
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${STEAM_BINARY_PATH}"
                    "$<TARGET_BUNDLE_DIR:ScreenPlayApp>/Contents/MacOS/${STEAM_BINARY}"
            # Create Steam App ID file
            COMMAND ${CMAKE_COMMAND} -E echo "672870" > "$<TARGET_BUNDLE_DIR:ScreenPlayApp>/Contents/MacOS/steam_appid.txt"
            COMMENT "Copying Steam files from ${STEAM_BINARY_PATH} to bundle...")

        # Print the paths for debugging
        message(STATUS "Steam root: ${STEAM_ROOT}")
        message(STATUS "Steam binary: ${STEAM_BINARY}")
        message(STATUS "Steam binary path: ${STEAM_BINARY_PATH}")
    elseif(WIN32)
        add_custom_command(
            TARGET ScreenPlayApp
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${STEAM_BINARY_PATH}" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

        file(WRITE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/steam_appid.txt" "672870")
    elseif(UNIX AND NOT APPLE)
        set(OUT_DIR "${CMAKE_BINARY_DIR}/ScreenPlay")
        add_custom_command(
            TARGET ScreenPlayApp
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${STEAM_BINARY_PATH}" "${OUT_DIR}")

        file(WRITE "${OUT_DIR}/steam_appid.txt" "672870")
    endif()
endif()

# Note: We need the variables in the lib and exe!
generate_cmake_variable_header(ScreenPlayApp)
generate_cmake_variable_header(ScreenPlayLib)

include(GNUInstallDirs)
install(
    TARGETS ScreenPlayApp
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

register_qt_main_deployment(ScreenPlayApp)
