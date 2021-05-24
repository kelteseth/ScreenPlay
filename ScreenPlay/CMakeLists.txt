project(ScreenPlay LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

find_package(
    Qt5
    COMPONENTS Quick
               QuickCompiler
               Widgets
               Gui
               Svg
               WebEngine
               LinguistTools
               WebSockets
    REQUIRED)

find_package(OpenSSL REQUIRED)
find_package(benchmark CONFIG REQUIRED)
find_package(doctest CONFIG REQUIRED)

if(WIN32)
    find_package(sentry CONFIG REQUIRED)
endif()

set(src
    main.cpp
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

set(l10n
    translations/ScreenPlay_de.ts
    translations/ScreenPlay_zh_cn.ts
    translations/ScreenPlay_pt_br.ts
    translations/ScreenPlay_en.ts
    translations/ScreenPlay_es.ts
    translations/ScreenPlay_fr.ts
    translations/ScreenPlay_ko.ts
    translations/ScreenPlay_ru.ts
    translations/ScreenPlay_vi.ts)

set_source_files_properties(${l10n} PROPERTIES OUTPUT_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/translations")
qt5_add_translation(qmFiles ${l10n})

qt5_add_big_resources(fonts fonts.qrc)

if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    qt5_add_resources(qml qml.qrc assets.qrc)
else()
    qtquick_compiler_add_resources(qml qml.qrc assets.qrc)
endif()

add_executable(${PROJECT_NAME} ${src} ${headers} ${resources} ${qml} ${fonts})

if(APPLE)
    #set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE true)
endif()

if(WIN32)
    # Icon
    target_sources(${PROJECT_NAME} PRIVATE ScreenPlay.rc)
    target_link_libraries(${PROJECT_NAME} PRIVATE sentry::sentry)

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

target_compile_definitions(${PROJECT_NAME} PRIVATE "GIT_VERSION=${GIT_VERSION}")

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE Qt5::Quick
            Qt5::Gui
            Qt5::Widgets
            Qt5::Core
            Qt5::WebEngine
            Qt5::WebSockets
            Qt5::Svg
            ScreenPlaySDK
            ScreenPlayUtil
            benchmark::benchmark
            benchmark::benchmark_main
            doctest::doctest)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE sentry::sentry)
endif()

if(APPLE)

    # Creates a ScreenPlay.app
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_NAME ${PROJECT_NAME}
        MACOSX_BUNDLE TRUE
        MACOSX_RPATH TRUE
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist
        MACOSX_FRAMEWORK_IDENTIFIER screen-play.app
        XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS "@loader_path/Libraries"
        RESOURCE "${RESOURCE_FILES}"
        XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME TRUE
        XCODE_ATTRIBUTE_EXECUTABLE_NAME ${PROJECT_NAME}
    )


    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/../Common/ffmpeg/ffmpeg
        ${CMAKE_BINARY_DIR}/bin/ScreenPlay.app/Contents/MacOS/ )

    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/../Common/ffmpeg/ffprobe
        ${CMAKE_BINARY_DIR}/bin/ScreenPlay.app/Contents/MacOS/ )


endif()
