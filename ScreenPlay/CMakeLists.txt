project(ScreenPlay LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

set(SOURCES
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
    main.qml
    qml/Community/Community.qml
    qml/Community/CommunityNavItem.qml
    qml/Community/XMLNewsfeed.qml
    qml/Create/Wizard.qml
    qml/Create/Create.qml
    qml/Create/Sidebar.qml
    qml/Create/StartInfo.qml
    qml/Create/StartInfoLinkImage.qml
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
    qml/Installed/InstalledWelcomeScreen.qml
    qml/Installed/Navigation.qml
    qml/Installed/ScreenPlayItem.qml
    qml/Installed/ScreenPlayItemImage.qml
    qml/Installed/Sidebar.qml
    qml/Monitors/DefaultVideoControls.qml
    qml/Monitors/Monitors.qml
    qml/Monitors/MonitorSelection.qml
    qml/Monitors/MonitorSelectionItem.qml
    qml/Monitors/MonitorsProjectSettingItem.qml
    qml/Monitors/SaveNotification.qml
    qml/Navigation/Navigation.qml
    qml/Navigation/WindowNavButton.qml
    qml/Navigation/WindowNavigation.qml
    qml/Settings/SettingBool.qml
    qml/Settings/Settings.qml
    qml/Settings/SettingsButton.qml
    qml/Settings/SettingsComboBox.qml
    qml/Settings/SettingsExpander.qml
    qml/Settings/SettingsHeader.qml
    qml/Settings/SettingsHorizontalSeperator.qml
    qml/Settings/SettingsPage.qml
    qml/Workshop/Workshop.qml)

set(TS_FILES
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

# Needed on macos
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)

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
               Gui
               Widgets
               WebSockets
               Svg
               LinguistTools
               Test)

qt_add_resources(RESOURCES Resources.qrc)
qt_add_big_resources(FONTS fonts.qrc)

add_library(
    ScreenPlayLib
    ${SOURCES}
    ${HEADER}
    ${RESOURCES}
    ${FONTS})

target_include_directories(
    ScreenPlayLib
    PUBLIC inc/public/
    PRIVATE src/)

target_link_libraries(
    ScreenPlayLib
    PUBLIC ScreenPlaySDK
           Threads::Threads
           Qt6::Quick
           Qt6::Gui
           Qt6::Widgets
           Qt6::Core
           Qt6::WebSockets
           Qt6::Svg
           ScreenPlayQmlplugin
           ScreenPlayUtil
           ScreenPlayUtilplugin
           SteamSDKQtEnums)

if(${SCREENPLAY_STEAM})
    target_compile_definitions(ScreenPlayLib PRIVATE SCREENPLAY_STEAM=1)
    target_link_libraries(ScreenPlayLib PUBLIC ScreenPlayWorkshopplugin ScreenPlayWorkshop)
endif()

target_include_directories(ScreenPlayLib PUBLIC)

qt_add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ScreenPlayLib)

if(${SCREENPLAY_TESTS})
    add_executable(tst_ScreenPlay tests/tst_main.cpp)
    target_link_libraries(tst_ScreenPlay PRIVATE ScreenPlayLib Qt6::Test)
    if(${SCREENPLAY_STEAM})
        target_compile_definitions(tst_ScreenPlay PRIVATE SCREENPLAY_STEAM=1)
        target_link_libraries(tst_ScreenPlay PUBLIC ScreenPlayWorkshopplugin ScreenPlayWorkshop)
    endif()
endif()

set_source_files_properties(${TS_FILES} PROPERTIES OUTPUT_LOCATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/translations")

# qt_add_lupdate does not work for some reason. Lets do it manually:
find_program(LUPDATE_EXECUTABLE lupdate)
foreach(_ts_file ${TS_FILES})
    message(STATUS "Update Translation: ${_ts_file}")
    execute_process(COMMAND ${LUPDATE_EXECUTABLE} -noobsolete -locations none -recursive ${CMAKE_CURRENT_SOURCE_DIR}/qml -ts
                            ${CMAKE_CURRENT_SOURCE_DIR}/${_ts_file} OUTPUT_QUIET)
endforeach()

qt_add_lrelease(${PROJECT_NAME} TS_FILES ${TS_FILES})

target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

qt_add_library(ScreenPlayQml STATIC)
qt_add_qml_module(
    ScreenPlayQml
    OUTPUT_DIRECTORY
    ${CMAKE_BINARY_DIR}/bin/ScreenPlayQml
    URI
    ScreenPlayQml
    VERSION
    1.0
    QML_FILES
    ${QML})

if(WIN32)
    target_link_libraries(ScreenPlayLib PUBLIC sentry::sentry)

    # Icon
    target_sources(${PROJECT_NAME} PRIVATE ScreenPlay.rc)

    # Needed for the installscript.qs Windows entry
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/assets/icons/app.ico ${CMAKE_BINARY_DIR}/bin/ COPYONLY)

    # Disable console window on Windows
    # https://stackoverflow.com/questions/8249028/how-do-i-keep-my-qt-c-program-from-opening-a-console-in-windows
    set_property(TARGET ${PROJECT_NAME} PROPERTY WIN32_EXECUTABLE true)

    # Copy ffmpeg. If the ffmpeg files are missing, start the install_dependencies_XXX for your system!
    file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/../Common/ffmpeg/*")
    foreach(filename ${files})
        configure_file(${filename} ${CMAKE_BINARY_DIR}/bin/ COPYONLY)
    endforeach()

    configure_file(${VCPKG_INSTALLED_PATH}/tools/sentry-native/crashpad_handler.exe ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ COPYONLY)
    configure_file(${VCPKG_INSTALLED_PATH}/tools/sentry-native/zlib1.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ COPYONLY)

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
    if(${SCREENPLAY_TESTS})
        add_custom_command(
            TARGET ${PROJECT_NAME}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/../Common/ffmpeg/ffmpeg ${CMAKE_BINARY_DIR}/bin/)

        add_custom_command(
            TARGET ${PROJECT_NAME}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/../Common/ffmpeg/ffprobe ${CMAKE_BINARY_DIR}/bin/)
    endif()

endif()
