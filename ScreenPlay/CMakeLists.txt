project(ScreenPlay LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

find_package(
    QT NAMES Qt6 Qt5
    COMPONENTS Quick Widgets Gui LinguistTools WebSockets WebEngine QuickCompiler
    REQUIRED)

find_package(
    Qt${QT_VERSION_MAJOR}
    COMPONENTS Quick Widgets Gui LinguistTools WebSockets WebEngine QuickCompiler
    REQUIRED)

message(STATUS "[PROJECT] QT_VERSION_MAJOR = ${QT_VERSION_MAJOR}")

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
qt_add_translation(qmFiles ${l10n})

if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    qt_add_resources(qml qml.qrc Resources.qrc)
else()
    qtquick_compiler_add_resources(qml qml.qrc Resources.qrc)
endif()

add_executable(${PROJECT_NAME} ${src} ${headers} ${resources} ${qml})

if(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE true)
endif()

if(WIN32)
    # Icon
    target_sources(${PROJECT_NAME} PRIVATE ScreenPlay.rc)

    # Disable console window on Windows
    # https://stackoverflow.com/questions/8249028/how-do-i-keep-my-qt-c-program-from-opening-a-console-in-windows
    set_property(TARGET ${PROJECT_NAME} PROPERTY WIN32_EXECUTABLE true)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE "GIT_VERSION=${GIT_VERSION}")

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE Qt${QT_VERSION_MAJOR}::Quick
            Qt${QT_VERSION_MAJOR}::Gui
            Qt${QT_VERSION_MAJOR}::Widgets
            Qt${QT_VERSION_MAJOR}::Core
            Qt${QT_VERSION_MAJOR}::WebSockets
            Qt${QT_VERSION_MAJOR}::WebEngine
            ScreenPlaySDK
            ScreenPlayUtil
            benchmark::benchmark
            benchmark::benchmark_main
            doctest::doctest)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE sentry::sentry)
endif()
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/assets/fonts)
configure_file(assets/fonts/NotoSansCJKkr-Regular.otf ${CMAKE_BINARY_DIR}/bin/assets/fonts COPYONLY)

# Copy ffmpeg. If the ffmpeg files are missing, start the install_dependencies_XXX for your system!
if(WIN32)

    file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/../Common/ffmpeg/*")
    foreach(filename ${files})
        configure_file(${filename} ${CMAKE_BINARY_DIR}/bin/ COPYONLY)
    endforeach()

    configure_file(${VCPKG_INSTALLED_PATH}/tools/sentry-native/crashpad_handler.exe ${CMAKE_BINARY_DIR}/bin/ COPYONLY)
endif()
