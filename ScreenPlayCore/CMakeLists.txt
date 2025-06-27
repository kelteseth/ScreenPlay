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
    qml/Grow.qml
    qml/GrowIconLink.qml
    qml/Headline.qml
    qml/HeadlineSection.qml
    qml/ImageSelector.qml
    qml/InstantPopup.js
    qml/LabelSlider.qml
    qml/LicenseSelector.qml
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
    src/archive.cpp
    src/contenttypes.cpp
    src/exitcodes.cpp
    src/licensemanager.cpp
    src/logginghandler.cpp
    src/processmanager.cpp
    src/projectfile.cpp
    src/steamenumsgenerated.cpp
    src/util.cpp)

set(HEADER
    # cmake-format: sort
    inc/public/ScreenPlayCore/archive.h
    inc/public/ScreenPlayCore/contenttypes.h
    inc/public/ScreenPlayCore/exitcodes.h
    inc/public/ScreenPlayCore/globalenums.h
    inc/public/ScreenPlayCore/HelpersCommon.h
    inc/public/ScreenPlayCore/licensemanager.h
    inc/public/ScreenPlayCore/ListPropertyHelper.h
    inc/public/ScreenPlayCore/logginghandler.h
    inc/public/ScreenPlayCore/processmanager.h
    inc/public/ScreenPlayCore/projectfile.h
    inc/public/ScreenPlayCore/PropertyHelpers.h
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

set(RESOURCES # cmake-format: sort
              assets/icons/attach_file.svg assets/icons/description.svg assets/icons/folder.svg)

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
