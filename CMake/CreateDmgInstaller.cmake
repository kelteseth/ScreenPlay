message(STATUS "[DMG INSTALLER ENABLED]: Configuring. This can take some time...")


set(CPACK_PACKAGE_VENDOR "Elias Steurer Tachiom")
set(CPACK_PACKAGE_NAME "ScreenPlay")
set(CPACK_PACKAGE_CONTACT "Elias Steurer <eli@kelteseth.com>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "ScreenPlay is an Open Source Live-Wallpaper app for Windows and OSX. ")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})

set(CPACK_GENERATOR "DragNDrop")
set(MACOSX_BUNDLE_BUNDLE_NAME ${CPACK_PACKAGE_NAME})
set(MACOSX_BUNDLE_BUNDLE_GUI_IDENTIFIER "app.screenplay.screenplay")
set(CPACK_DMG_VOLUME_NAME ${CPACK_PACKAGE_NAME})
set(CPACK_SYSTEM_NAME "OSX")

set(CPACK_COMPONENTS_GROUPING IGNORE)
set(CPACK_PACKAGING_INSTALL_PREFIX "/")
set(CPACK_PACKAGE_FILE_NAME "ScreenPlay-Installer")
set(CPACK_COMPONENTS_ALL "ScreenPlayComponent")
set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
set(CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE ON)

set(MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
set(MACOSX_BUNDLE_ICON_FILE "${CMAKE_SOURCE_DIR}/ScreenPlay/assets/icons/ScreenPlay.icns")
set(CPACK_DMG_DS_STORE_SETUP_SCRIPT "${CMAKE_SOURCE_DIR}/CMake/DS_Store.scpt") 
set(CPACK_DMG_BACKGROUND_IMAGE "${CMAKE_SOURCE_DIR}/CMake/dmg_background.png")
set(MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/CMake/MacOSXBundleInfo.plist.in")

set_source_files_properties(${CMAKE_SOURCE_DIR}/CMake/exampleApp.icns PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

# Install all files from /bin
install(
    DIRECTORY "${CMAKE_BINARY_DIR}/bin/"
    COMPONENT ScreenPlayComponent
    DESTINATION "./")

include(CPack)
