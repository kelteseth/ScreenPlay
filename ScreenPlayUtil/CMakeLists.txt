project(ScreenPlayUtil LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

find_package(
    Qt6
    COMPONENTS Core Quick
    REQUIRED)

set(QML
    qml/CloseIcon.qml
    qml/ColorPicker.qml
    qml/Dialog.qml
    qml/Dialogs/CriticalError.qml
    qml/Dialogs/MonitorConfiguration.qml
    qml/Dialogs/SteamNotAvailable.qml
    qml/FileSelector.qml
    qml/Grow.qml
    qml/GrowIconLink.qml
    qml/Headline.qml
    qml/HeadlineSection.qml
    qml/ImageSelector.qml
    qml/LicenseSelector.qml
    qml/ModalBackgroundBlur.qml
    qml/MouseHoverBlocker.qml
    qml/Popup.qml
    qml/RippleEffect.qml
    qml/Search.qml
    qml/Shake.qml
    qml/FileDropAnimation.qml
    qml/Slider.qml
    qml/Tag.qml
    qml/TagSelector.qml
    qml/TextField.qml
    qml/TrayIcon.qml
    qml/JSUtil.js)

set(SOURCES inc/public/ScreenPlayUtil/httpfileserver.cpp src/contenttypes.cpp src/util.cpp)

set(HEADER
    inc/public/ScreenPlayUtil/AutoPropertyHelpers.h
    inc/public/ScreenPlayUtil/ConstRefPropertyHelpers.h
    inc/public/ScreenPlayUtil/contenttypes.h
    inc/public/ScreenPlayUtil/HelpersCommon.h
    inc/public/ScreenPlayUtil/httpfileserver.h
    inc/public/ScreenPlayUtil/ListPropertyHelper.h
    inc/public/ScreenPlayUtil/projectfile.h
    inc/public/ScreenPlayUtil/PropertyHelpers.h
    inc/public/ScreenPlayUtil/PtrPropertyHelpers.h
    inc/public/ScreenPlayUtil/SingletonHelper.h
    inc/public/ScreenPlayUtil/util.h)

set(RESOURCES
    assets/icons/folder.svg
    assets/icons/description.svg
    assets/icons/attach_file.svg)

qt_add_library(
    ${PROJECT_NAME}
    STATIC
    ${SOURCES}
    ${HEADER})

qt_add_qml_module(
    ${PROJECT_NAME}
    OUTPUT_DIRECTORY
    ${SCREENPLAY_QML_MODULES_PATH}/${PROJECT_NAME}
    RESOURCE_PREFIX /qml
    URI
    ${PROJECT_NAME}
    VERSION
    1.0
    QML_FILES
    ${QML}
    RESOURCES
    ${RESOURCES})

find_path(CPP_HTTPLIB_INCLUDE_DIRS "httplib.h")
target_include_directories(${PROJECT_NAME} PUBLIC ${CPP_HTTPLIB_INCLUDE_DIRS})

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC inc/public/
    PRIVATE src/)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Quick)

if(WIN32)
    # Used for query windows monitor data
    target_link_libraries(${PROJECT_NAME} PUBLIC shcore.lib)
endif()
