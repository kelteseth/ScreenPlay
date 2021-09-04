project(ScreenPlay LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core)
find_package(OpenSSL REQUIRED)
find_package(benchmark CONFIG REQUIRED)
find_package(doctest CONFIG REQUIRED)

# CURL must be included before sentry because sentry needs the module and does not include it itself on macos...
find_package(CURL CONFIG REQUIRED)
find_package(sentry CONFIG REQUIRED)

set(src
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

set(headers
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

list(
    APPEND
    L10N_LIST
    # cmake-format: sortable
     ${CMAKE_CURRENT_SOURCE_DIR}/translations/ScreenPlay_de.ts
     ${CMAKE_CURRENT_SOURCE_DIR}/translations/ScreenPlay_zh_cn.ts
     ${CMAKE_CURRENT_SOURCE_DIR}/translations/ScreenPlay_pt_br.ts
     ${CMAKE_CURRENT_SOURCE_DIR}/translations/ScreenPlay_en.ts
     ${CMAKE_CURRENT_SOURCE_DIR}/translations/ScreenPlay_es.ts
     ${CMAKE_CURRENT_SOURCE_DIR}/translations/ScreenPlay_fr.ts
     ${CMAKE_CURRENT_SOURCE_DIR}/translations/ScreenPlay_ko.ts
     ${CMAKE_CURRENT_SOURCE_DIR}/translations/ScreenPlay_ru.ts
     ${CMAKE_CURRENT_SOURCE_DIR}/translations/ScreenPlay_vi.ts)

include(QtUpdateTranslations)
qt_update_translations("${CMAKE_CURRENT_SOURCE_DIR}/qml" "${L10N_LIST}")

# Needed on macos
find_package(Threads REQUIRED)

find_package(
    Qt${QT_VERSION_MAJOR}
    COMPONENTS Core
               Quick
               Gui
               Widgets
               WebSockets
               Svg
               LinguistTools
               WebEngine
               QuickCompiler
               Test)

if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    qt_add_resources(resources ScreenPlayAssets.qrc ScreenPlayQML.qrc)
else()
    qtquick_compiler_add_resources(resources ScreenPlayAssets.qrc ScreenPlayQML.qrc)
endif()

add_library(ScreenPlayLib ${src} ${headers} ${resources} ${resources} ${fonts})

target_include_directories(ScreenPlayLib PUBLIC ./ src/ )


target_link_libraries(
    ScreenPlayLib
    PUBLIC ScreenPlaySDK
            ScreenPlayUtil
            benchmark::benchmark
            benchmark::benchmark_main
            doctest::doctest
            sentry::sentry
            Threads::Threads
            Qt${QT_VERSION_MAJOR}::Quick
            Qt${QT_VERSION_MAJOR}::Gui
            Qt${QT_VERSION_MAJOR}::Widgets
            Qt${QT_VERSION_MAJOR}::Core
            Qt${QT_VERSION_MAJOR}::WebSockets
            Qt${QT_VERSION_MAJOR}::Svg
            Qt5::WebEngine)

add_executable(tst_ScreenPlay  tests/tst_main.cpp)
target_link_libraries(tst_ScreenPlay PRIVATE ScreenPlayLib Qt5::Test)

add_executable(${PROJECT_NAME}  main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ScreenPlayLib)

qt5_add_translation(qmFiles ${l10n})


qt_add_big_resources(fonts fonts.qrc)

target_compile_definitions(${PROJECT_NAME} PRIVATE "GIT_VERSION=${GIT_VERSION}")

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
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/../Common/ffmpeg/ffmpeg
                    ${CMAKE_BINARY_DIR}/bin/)

        add_custom_command(
            TARGET ${PROJECT_NAME}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/../Common/ffmpeg/ffprobe
                    ${CMAKE_BINARY_DIR}/bin/)
    endif()

    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${VCPKG_INSTALLED_PATH}/tools/sentry-native/crashpad_handler
                ${CMAKE_BINARY_DIR}/bin/ScreenPlay.app/Contents/MacOS/)

endif()
