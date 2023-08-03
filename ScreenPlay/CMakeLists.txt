# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
project(ScreenPlay LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

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
    src/screenplaywallpaper.cpp
    src/screenplaywidget.cpp
    src/sdkconnection.cpp
    src/settings.cpp
    src/util.cpp
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
    inc/public/ScreenPlay/screenplaywallpaper.h
    inc/public/ScreenPlay/screenplaywidget.h
    inc/public/ScreenPlay/sdkconnection.h
    inc/public/ScreenPlay/settings.h
    inc/public/ScreenPlay/util.h
    inc/public/ScreenPlay/wizards.h)

set(QML
    # cmake-format: sort
    main.qml
    qml/Community/Community.qml
    qml/Community/CommunityNavItem.qml
    qml/Community/XMLNewsfeed.qml
    qml/Create/Create.qml
    qml/Create/CreateSidebar.qml
    qml/Create/StartInfo.qml
    qml/Create/StartInfoLinkImage.qml
    qml/Create/Wizard.qml
    qml/Create/Wizards/GifWallpaper.qml
    qml/Create/Wizards/HTMLWallpaper.qml
    qml/Create/Wizards/HTMLWidget.qml
    qml/Create/Wizards/Importh264/Importh264.qml
    qml/Create/Wizards/Importh264/Importh264Convert.qml
    qml/Create/Wizards/Importh264/Importh264Init.qml
    qml/Create/Wizards/ImportVideoAndConvert/CreateWallpaper.qml
    qml/Create/Wizards/ImportVideoAndConvert/CreateWallpaperInit.qml
    qml/Create/Wizards/ImportVideoAndConvert/CreateWallpaperResult.qml
    qml/Create/Wizards/ImportVideoAndConvert/CreateWallpaperVideoImportConvert.qml
    qml/Create/Wizards/ImportWebm/ImportWebm.qml
    qml/Create/Wizards/ImportWebm/ImportWebmConvert.qml
    qml/Create/Wizards/ImportWebm/ImportWebmInit.qml
    qml/Create/Wizards/QMLWallpaper.qml
    qml/Create/Wizards/QMLWidget.qml
    qml/Create/Wizards/WebsiteWallpaper.qml
    qml/Create/Wizards/WizardPage.qml
    qml/Create/WizardsFiles/QMLWallpaperMain.qml
    qml/Create/WizardsFiles/QMLWidgetMain.qml
    qml/Installed/Installed.qml
    qml/Installed/InstalledNavigation.qml
    qml/Installed/InstalledWelcomeScreen.qml
    qml/Installed/ScreenPlayItem.qml
    qml/Installed/ScreenPlayItemImage.qml
    qml/Installed/Sidebar.qml
    qml/Monitors/DefaultVideoControls.qml
    qml/Monitors/Monitors.qml
    qml/Monitors/MonitorSelection.qml
    qml/Monitors/MonitorSelectionItem.qml
    qml/Monitors/MonitorsProjectSettingItem.qml
    qml/Monitors/SaveNotification.qml
    qml/Navigation/ExitPopup.qml
    qml/Navigation/Navigation.qml
    qml/Settings/SettingBool.qml
    qml/Settings/Settings.qml
    qml/Settings/SettingsButton.qml
    qml/Settings/SettingsComboBox.qml
    qml/Settings/SettingsExpander.qml
    qml/Settings/SettingsHeader.qml
    qml/Settings/SettingsHorizontalSeperator.qml
    qml/Settings/SettingsPage.qml
    qml/TrayIcon.qml
    qml/Workshop/Workshop.qml)

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
    assets/icons/brand_twitter.svg
    assets/icons/exclamation-triangle-solid.svg
    assets/icons/font-awsome/close.svg
    assets/icons/font-awsome/frown-o.svg
    assets/icons/font-awsome/patreon-brands.svg
    assets/icons/icon_arrow_left.svg
    assets/icons/icon_arrow_right.svg
    assets/icons/icon_build.svg
    assets/icons/icon_cake.afdesign
    assets/icons/icon_cake.svg
    assets/icons/icon_close.svg
    assets/icons/icon_code.svg
    assets/icons/icon_community.svg
    assets/icons/icon_delete.svg
    assets/icons/icon_document.svg
    assets/icons/icon_done.svg
    assets/icons/icon_download.svg
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
    assets/icons/icon_contains_audio.svg
    assets/icons/icon_movie.svg
    assets/icons/icon_new_releases.svg
    assets/icons/icon_open_in_new.svg
    assets/icons/icon_pause.svg
    assets/icons/icon_people.svg
    assets/icons/icon_play.svg
    assets/icons/icon_plus.svg
    assets/icons/icon_report_problem.svg
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
    assets/icons/icon_video_settings_black_24dp.svg
    assets/icons/icon_volume.svg
    assets/icons/icon_volume_mute.svg
    assets/icons/icon_volume_up.svg
    assets/icons/icon_widgets.svg
    assets/icons/icon_window.svg
    assets/icons/item_banner_new.svg
    assets/icons/monitor_setup.svg
    assets/icons/steam_default_avatar.png
    assets/images/Early_Access.png
    assets/images/Intro.png
    assets/images/Intro_PC.png
    assets/images/Intro_shine.png
    assets/images/mask_round.svg
    assets/images/mask_workshop.png
    assets/images/missingPreview.png
    assets/images/noisy-texture-3.png
    assets/images/noisy-texture.png
    assets/images/scale_window_indicator.png
    assets/images/steam_offline.png
    assets/images/trayIcon_osx.png
    assets/images/trayIcon_windows.png
    assets/images/Window.svg
    assets/licenses/Apache2.txt
    assets/licenses/OFL.txt
    assets/macos/app.screenplay.plist
    assets/particle/backgroundGlow.png
    assets/particle/dot.png
    assets/shader/movingcolorramp.fsh
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
    assets/wizards/QmlProject.qmlproject
    assets/WorkshopPreview.html
    legal/DataProtection.txt
    legal/gpl-3.0.txt
    legal/lgpl-2.1.txt
    legal/OFL.txt
    legal/OpenSSL.txt
    profiles.json
    qml/Create/WizardsFiles/HTMLWallpaperMain.html
    qml/Create/WizardsFiles/HTMLWidgetMain.html
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

add_library(ScreenPlayApp STATIC)
target_include_directories(ScreenPlayApp PUBLIC src/ inc/public/ScreenPlay)

qt_add_qml_module(
    ScreenPlayApp
    URI
    ScreenPlayApp
    OUTPUT_DIRECTORY
    ${SCREENPLAY_QML_MODULES_PATH}/${PROJECT_NAME}App
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

target_link_libraries(
    ScreenPlayApp
    PUBLIC ScreenPlaySDK
           LibArchive::LibArchive
           ScreenPlayUtil
           ScreenPlayUtilplugin
           QArchive
           Plausibleplugin
           SteamSDKQtEnums
           Threads::Threads
           Qt6::Quick
           Qt6::Gui
           Qt6::Widgets
           Qt6::Core
           Qt6::WebSockets
           Qt6::Svg
           Qt6::QuickControls2
           Qt6::Xml)

if(${SCREENPLAY_STEAM})
    target_compile_definitions(ScreenPlayApp PRIVATE SCREENPLAY_STEAM)
    target_link_libraries(ScreenPlayApp PUBLIC ScreenPlayWorkshopplugin ScreenPlayWorkshop)
endif()

qt_add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ScreenPlayApp ScreenPlayAppplugin)

if(${SCREENPLAY_TESTS})
    add_executable(tst_ScreenPlay tests/tst_main.cpp)
    target_link_libraries(tst_ScreenPlay PRIVATE ScreenPlayApp ScreenPlayAppplugin Qt6::Test)
    if(${SCREENPLAY_STEAM})
        target_compile_definitions(tst_ScreenPlay PRIVATE SCREENPLAY_STEAM)
        target_link_libraries(tst_ScreenPlay PUBLIC ScreenPlayWorkshopplugin ScreenPlayWorkshop)
    endif()
endif()

if(APPLE AND NOT OSX_BUNDLE)
    set_source_files_properties(
        ${TS_FILES} PROPERTIES OUTPUT_LOCATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.app/Contents/Resources/translations")
else()
    set_source_files_properties(${TS_FILES} PROPERTIES OUTPUT_LOCATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/translations")
endif()

qt_add_lrelease(${PROJECT_NAME} TS_FILES ${TS_FILES})

target_include_directories(
    ScreenPlayApp
    PUBLIC inc/public/
    PRIVATE src/)

if(WIN32
   OR UNIX
   AND NOT APPLE)
    include(CopyRecursive)
    set(FONTS_OUT_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/fonts)
    file(MAKE_DIRECTORY ${FONTS_OUT_DIR})
    copy_recursive(${CMAKE_CURRENT_SOURCE_DIR}/assets/fonts ${FONTS_OUT_DIR} "*.ttf")
    copy_recursive(${CMAKE_CURRENT_SOURCE_DIR}/assets/fonts ${FONTS_OUT_DIR} "*.otf")
endif()

if(WIN32)
    target_link_libraries(ScreenPlayApp PUBLIC CURL::libcurl sentry::sentry)

    # Icon
    target_sources(${PROJECT_NAME} PRIVATE ScreenPlay.rc)

    # Needed for the installscript.qs Windows entry
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/assets/icons/app.ico ${CMAKE_BINARY_DIR}/bin/ COPYONLY)

    # Disable console window on Windows
    # https://stackoverflow.com/questions/8249028/how-do-i-keep-my-qt-c-program-from-opening-a-console-in-windows
    set_property(TARGET ${PROJECT_NAME} PROPERTY WIN32_EXECUTABLE true)

    # Copy ffmpeg. If the ffmpeg files are missing, start the install_dependencies_XXX for your system!
    file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/ffmpeg/*")
    foreach(filename ${files})
        configure_file(${filename} ${CMAKE_BINARY_DIR}/bin/ COPYONLY)
    endforeach()

    configure_file(${VCPKG_INSTALLED_PATH}/tools/sentry-native/crashpad_handler.exe ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ COPYONLY)
    configure_file(${VCPKG_INSTALLED_PATH}/tools/sentry-native/zlib1.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ COPYONLY)

    configure_file(${VCPKG_BIN_PATH}/libssl-3-x64.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ COPYONLY)
    configure_file(${VCPKG_BIN_PATH}/libcrypto-3-x64.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ COPYONLY)

endif()

if(APPLE AND OSX_BUNDLE)
    # Set the installation destination
    install(TARGETS "ScreenPlay" DESTINATION /Applications)

    include(InstallRequiredSystemLibraries)
    set(CPACK_GENERATOR "Bundle")
    set(CPACK_BINARY_DRAGNDROP ON)
    set(CPACK_BUNDLE_NAME "ScreenPlay")
    set(CPACK_BUNDLE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/assets/icons/app.ico")
    set(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/assets/icons/app.ico")
    set(CPACK_BUNDLE_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist")
    set(CPACK_BUNDLE_APPLE_ENTITLEMENTS "${CMAKE_CURRENT_SOURCE_DIR}/entitlements.plist")
    set(CPACK_PACKAGE_FILE_NAME "ScreenPlay-0.15")
    include(CPack)

    # Install all files from /bin
    install(
        DIRECTORY "${CMAKE_BINARY_DIR}/bin/"
        COMPONENT ScreenPlay
        DESTINATION "./")

endif()

if(APPLE AND NOT OSX_BUNDLE)
    # Creates a ScreenPlay.app
    set_target_properties(
        ${PROJECT_NAME}
        PROPERTIES OUTPUT_NAME ${PROJECT_NAME}
                   MACOSX_BUNDLE TRUE
                   MACOSX_RPATH TRUE
                   MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist
                   MACOSX_FRAMEWORK_IDENTIFIER screen-play.app
                   XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS "@loader_path/Libraries"
                   RESOURCE "${RESOURCE_FILES}"
                   XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME TRUE
                   XCODE_ATTRIBUTE_EXECUTABLE_NAME ${PROJECT_NAME})

    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/ffmpeg/ffmpeg
                ${CMAKE_BINARY_DIR}/bin/ScreenPlay.app/Contents/MacOS/)

    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/ffmpeg/ffprobe
                ${CMAKE_BINARY_DIR}/bin/ScreenPlay.app/Contents/MacOS/)

    # tst_ScreenPlay needs ffmpeg in the base path
    if(${SCREENPLAY_TESTS})
        add_custom_command(
            TARGET ${PROJECT_NAME}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/ffmpeg/ffmpeg ${CMAKE_BINARY_DIR}/bin/)

        add_custom_command(
            TARGET ${PROJECT_NAME}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/ffmpeg/ffprobe ${CMAKE_BINARY_DIR}/bin/)
    endif()

    # fonts
    include(CopyRecursive)
    set(FONTS_OUT_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ScreenPlay.app/Contents/Resources/fonts/)
    file(MAKE_DIRECTORY ${FONTS_OUT_DIR})
    copy_recursive(${CMAKE_CURRENT_SOURCE_DIR}/assets/fonts ${FONTS_OUT_DIR} "*.ttf")
    copy_recursive(${CMAKE_CURRENT_SOURCE_DIR}/assets/fonts ${FONTS_OUT_DIR} "*.otf")

endif()

# ##### USE CMAKE VARIABLES IN CODE #####
include(GenerateCMakeVariableHeader)
# Note: Must not be ${PROJECT_NAME}, but ScreenPlayApp to work!
generate_cmake_variable_header(ScreenPlayApp)
