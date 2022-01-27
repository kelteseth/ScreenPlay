project(ScreenPlay LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

set(SOURCES
    # cmake-format: sortable
    app.cpp
    src/globalvariables.cpp
    src/createimportvideo.cpp
    src/installedlistmodel.cpp
    src/monitorlistmodel.cpp
    src/screenplaywallpaper.cpp
    src/screenplaywidget.cpp
    src/settings.cpp
    src/profilelistmodel.cpp
    src/installedlistfilter.cpp
    src/projectsettingslistmodel.cpp
    src/screenplaymanager.cpp
    src/sdkconnection.cpp
    src/util.cpp
    src/create.cpp
    src/wizards.cpp)

set(HEADER
    # cmake-format: sortable
    app.h
    src/globalvariables.h
    src/createimportvideo.h
    src/createimportstates.h
    src/installedlistmodel.h
    src/monitorlistmodel.h
    src/screenplaywallpaper.h
    src/screenplaywidget.h
    src/settings.h
    src/profilelistmodel.h
    src/profile.h
    src/installedlistfilter.h
    src/projectsettingslistmodel.h
    src/screenplaymanager.h
    src/sdkconnection.h
    src/util.h
    src/create.h
    src/wizards.h)

set(QML
    # cmake-format: sortable
    main.qml
    qml/Create/Create.qml
    qml/Create/Wizard.qml
    qml/Create/StartInfo.qml
    qml/Installed/Installed.qml
    qml/Installed/ScreenPlayItem.qml
    qml/Installed/ScreenPlayItemImage.qml
    qml/Installed/Sidebar.qml
    qml/Installed/InstalledWelcomeScreen.qml
    qml/Community/Community.qml
    qml/Community/XMLNewsfeed.qml
    qml/Settings/SettingBool.qml
    qml/Settings/Settings.qml
    qml/Settings/SettingsButton.qml
    qml/Settings/SettingsComboBox.qml
    qml/Settings/SettingsExpander.qml
    qml/Settings/SettingsHeader.qml
    qml/Settings/SettingsHorizontalSeperator.qml
    qml/Workshop/Workshop.qml
    qml/Monitors/Monitors.qml
    qml/Monitors/MonitorSelection.qml
    qml/Monitors/MonitorSelectionItem.qml
    qml/Monitors/MonitorsProjectSettingItem.qml
    qml/Navigation/Navigation.qml
    qml/Navigation/NavigationItem.qml
    qml/Navigation/WindowNavButton.qml
    qml/Navigation/WindowNavigation.qml
    qml/Monitors/DefaultVideoControls.qml
    qml/Common/TagSelector.qml
    qml/Common/Tag.qml
    qml/Common/Popup.qml
    qml/Common/Dialog.qml
    qml/Common/ImageSelector.qml
    qml/Common/Slider.qml
    qml/Common/RippleEffect.qml
    qml/Common/Shake.qml
    qml/Common/Grow.qml
    qml/Common/GrowIconLink.qml
    qml/Common/CloseIcon.qml
    qml/Common/Headline.qml
    qml/Settings/SettingsPage.qml
    qml/Community/CommunityNavItem.qml
    qml/Workshop/ScreenPlayItem.qml
    qml/Workshop/ScreenPlayItemImage.qml
    qml/Workshop/Background.qml
    qml/Workshop/WorkshopInstalled.qml
    qml/Workshop/WorkshopItem.qml
    qml/Workshop/Sidebar.qml
    qml/Workshop/Navigation.qml
    qml/Workshop/PopupOffline.qml
    qml/Workshop/upload/PopupSteamWorkshopAgreement.qml
    qml/Workshop/upload/UploadProject.qml
    qml/Workshop/upload/UploadProjectBigItem.qml
    qml/Workshop/upload/UploadProjectItem.qml
    qml/Monitors/SaveNotification.qml
    qml/Common/TrayIcon.qml
    qml/Installed/Navigation.qml
    qml/Common/Search.qml
    qml/Common/MouseHoverBlocker.qml
    qml/Create/Wizards/HTMLWallpaper.qml
    qml/Create/Wizards/HTMLWidget.qml
    qml/Create/Wizards/QMLWidget.qml
    qml/Create/Wizards/QMLWallpaper.qml
    qml/Create/Wizards/ImportVideoAndConvert/CreateWallpaper.qml
    qml/Create/Wizards/ImportVideoAndConvert/CreateWallpaperInit.qml
    qml/Create/Wizards/ImportVideoAndConvert/CreateWallpaperResult.qml
    qml/Create/Wizards/ImportVideoAndConvert/CreateWallpaperVideoImportConvert.qml
    qml/Create/Sidebar.qml
    qml/Create/Wizards/ImportWebm/ImportWebm.qml
    qml/Create/Wizards/ImportWebm/ImportWebmConvert.qml
    qml/Create/Wizards/ImportWebm/ImportWebmInit.qml
    qml/Common/Dialogs/MonitorConfiguration.qml
    qml/Common/Dialogs/SteamNotAvailable.qml
    qml/Create/Wizards/WizardPage.qml
    qml/Create/Wizards/GifWallpaper.qml
    qml/Common/TextField.qml
    qml/Common/HeadlineSection.qml
    qml/Create/Wizards/WebsiteWallpaper.qml
    qml/Common/FileSelector.qml
    qml/Create/WizardsFiles/QMLWidgetMain.qml
    qml/Create/WizardsFiles/QMLWallpaperMain.qml
    qml/Common/LicenseSelector.qml
    qml/Common/ModalBackgroundBlur.qml
    qml/Common/Util.js
    qml/Common/Dialogs/CriticalError.qml
    qml/Common/ColorPicker.qml
    qml/Create/StartInfoLinkImage.qml
    qml/Workshop/SteamProfile.qml
    qml/Workshop/SteamWorkshop.qml
    qml/Workshop/Forum.qml
    qml/Workshop/SteamWorkshopStartPage.qml
    qml/Create/Wizards/Importh264/Importh264.qml
    qml/Create/Wizards/Importh264/Importh264Convert.qml
    qml/Create/Wizards/Importh264/Importh264Init.qml)

set(TS_FILES
    # cmake-format: sortable
    translations/ScreenPlay_.ts
    translations/ScreenPlay_tr_TR.ts
    translations/ScreenPlay_it_IT.ts
    translations/ScreenPlay_nl_NL.ts
    translations/ScreenPlay_de_DE.ts
    translations/ScreenPlay_pl_PL.ts
    translations/ScreenPlay_zh_CN.ts
    translations/ScreenPlay_pt_BR.ts
    translations/ScreenPlay_es_ES.ts
    translations/ScreenPlay_fr_FR.ts
    translations/ScreenPlay_ko_KR.ts
    translations/ScreenPlay_ru_RU.ts
    translations/ScreenPlay_vi_VN.ts)

# Needed on macos
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(doctest CONFIG REQUIRED)

# CURL must be included before sentry because sentry needs the module and does not include it itself on macos...
find_package(CURL CONFIG REQUIRED)
find_package(sentry CONFIG REQUIRED)

find_package(
    Qt6
    COMPONENTS Core
               Quick
               Gui
               Widgets
               WebSockets
               Svg
               LinguistTools
               Test)

qt_add_resources(RESOURCES Resources.qrc)
qt_add_big_resources(FONTS fonts.qrc)


add_library(ScreenPlayLib ${SOURCES} ${HEADER} ${RESOURCES} ${FONTS})

target_link_libraries(
    ScreenPlayLib
    PUBLIC ScreenPlaySDK
           ScreenPlayUtil
           doctest::doctest
           sentry::sentry
           Threads::Threads
           Qt6::Quick
           Qt6::Gui
           Qt6::Widgets
           Qt6::Core
           Qt6::WebSockets
           Qt6::Svg
           ScreenPlayQmlplugin
           SteamSDKQtEnums)

if(${TESTS_ENABLED})
    add_executable(tst_ScreenPlay tests/tst_main.cpp)
    target_link_libraries(tst_ScreenPlay PRIVATE ScreenPlayLib Qt6::Test)
endif()

qt_add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ScreenPlayLib)

set_source_files_properties(${TS_FILES}
    PROPERTIES OUTPUT_LOCATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/translations")

# qt_add_lupdate does not work for some reason. Lets do it manually:
find_program(LUPDATE_EXECUTABLE lupdate)
foreach(_ts_file ${TS_FILES})
    message(STATUS "Update Translation: ${_ts_file}")
    execute_process(COMMAND ${LUPDATE_EXECUTABLE} -noobsolete -locations none -recursive ${CMAKE_CURRENT_SOURCE_DIR}/qml -ts ${CMAKE_CURRENT_SOURCE_DIR}/${_ts_file} OUTPUT_QUIET)
endforeach()

qt_add_lrelease(
    ${PROJECT_NAME}
    TS_FILES
    ${TS_FILES}
    )

target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

qt_add_library(ScreenPlayQml STATIC)
qt_add_qml_module(
    ScreenPlayQml
    URI
    ScreenPlayQml
    VERSION
    1.0
    QML_FILES
    ${QML})

if(WIN32)
    # Icon
    target_sources(${PROJECT_NAME} PRIVATE ScreenPlay.rc)

    # Disable console window on Windows
    # https://stackoverflow.com/questions/8249028/how-do-i-keep-my-qt-c-program-from-opening-a-console-in-windows
    set_property(TARGET ${PROJECT_NAME} PROPERTY WIN32_EXECUTABLE true)

    # Copy ffmpeg. If the ffmpeg files are missing, start the install_dependencies_XXX for your system!
    file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/../Common/ffmpeg/*")
    foreach(filename ${files})
        configure_file(${filename} ${CMAKE_BINARY_DIR}/bin/ COPYONLY)
    endforeach()

    configure_file(${VCPKG_INSTALLED_PATH}/tools/sentry-native/crashpad_handler.exe ${CMAKE_BINARY_DIR}/bin/ COPYONLY)

endif()

if(APPLE)
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

    # Needed for the Steam Workshop qml plugin
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/ScreenPlay.app/Contents/MacOS/Workshop)

    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/../Common/ffmpeg/ffmpeg
                ${CMAKE_BINARY_DIR}/bin/ScreenPlay.app/Contents/MacOS/)

    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/../Common/ffmpeg/ffprobe
                ${CMAKE_BINARY_DIR}/bin/ScreenPlay.app/Contents/MacOS/)

    # tst_ScreenPlay needs ffmpeg in the base path
    if(${TESTS_ENABLED})
        add_custom_command(
            TARGET ${PROJECT_NAME}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/../Common/ffmpeg/ffmpeg ${CMAKE_BINARY_DIR}/bin/)

        add_custom_command(
            TARGET ${PROJECT_NAME}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/../Common/ffmpeg/ffprobe ${CMAKE_BINARY_DIR}/bin/)
    endif()

    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${VCPKG_INSTALLED_PATH}/tools/sentry-native/crashpad_handler
                ${CMAKE_BINARY_DIR}/bin/ScreenPlay.app/Contents/MacOS/)

endif()
