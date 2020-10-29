project(ScreenPlay LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

find_package(
    Qt5
    COMPONENTS Quick QuickCompiler Widgets Gui WebEngine LinguistTools
    REQUIRED)

find_package(OpenSSL REQUIRED)
if(WIN32)
    find_package(sentry CONFIG REQUIRED)
endif()

set(src
    main.cpp
    app.cpp
    ../Common/qt-google-analytics/ganalytics.cpp
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
    src/create.cpp)

set(headers
    app.h
    ../Common/qt-google-analytics/ganalytics.h
    src/globalvariables.h
    src/createimportvideo.h
    src/installedlistmodel.h
    src/monitorlistmodel.h
    src/screenplaywallpaper.h
    src/screenplaywidget.h
    src/settings.h
    src/profilelistmodel.h
    src/profile.h
    src/projectfile.h
    src/installedlistfilter.h
    src/projectsettingslistmodel.h
    src/screenplaymanager.h
    src/sdkconnection.h
    src/util.h
    src/create.h)

set(l10n
    translations/ScreenPlay_de.ts
    translations/ScreenPlay_zh_cn.ts
    translations/ScreenPlay_en.ts
    translations/ScreenPlay_es.ts
    translations/ScreenPlay_fr.ts
    translations/ScreenPlay_ko.ts
    translations/ScreenPlay_ru.ts
    translations/ScreenPlay_vi.ts)

set_source_files_properties(${l10n} PROPERTIES OUTPUT_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/translations")
qt5_add_translation(qmFiles ${l10n})

if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    qt5_add_resources(qml qml.qrc resources.qrc)
else()
    qtquick_compiler_add_resources(qml qml.qrc resources.qrc)
endif()

add_executable(${PROJECT_NAME} ${src} ${headers} ${resources} ${qml})

if(WIN32)
    # Icon
    target_sources(${PROJECT_NAME} PRIVATE ScreenPlay.rc)

    # Disable console window on Windows
    # https://stackoverflow.com/questions/8249028/how-do-i-keep-my-qt-c-program-from-opening-a-console-in-windows
    set_property(TARGET ${PROJECT_NAME} PROPERTY WIN32_EXECUTABLE true)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE "GIT_VERSION=${GIT_VERSION}")

target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Quick Qt5::Gui Qt5::Widgets Qt5::Core Qt5::WebEngine ScreenPlaySDK)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE sentry::sentry)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${QT_TELEMTRY_INCLUDE})

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/assets/fonts)
configure_file(assets/fonts/NotoSansCJKkr-Regular.otf ${CMAKE_BINARY_DIR}/bin/assets/fonts COPYONLY)

# Copy ffmpeg. If the ffmpeg files are missing, start the install_dependencies_XXX for your system!
if(WIN32)

    file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/../Common/ffmpeg/*")
    foreach(filename ${files})
        configure_file(${filename} ${CMAKE_BINARY_DIR}/bin/ COPYONLY)
    endforeach()

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../Common/vcpkg/installed/x64-windows/tools/sentry-native/crashpad_handler.exe
                   ${CMAKE_BINARY_DIR}/bin/ COPYONLY)
elseif(APPLE)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../Common/ffmpeg/ffmpeg ${CMAKE_BINARY_DIR}/bin/ COPYONLY)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../Common/ffmpeg/ffprobe ${CMAKE_BINARY_DIR}/bin/ COPYONLY)
endif()
