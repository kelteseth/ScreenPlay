message(STATUS "[DMG INSTALLER ENABLED]: Configuring. This can take some time...")

set(CPACK_GENERATOR "DragNDrop")
set(CPACK_DMG_FORMAT "UDBZ")
set(CPACK_DMG_VOLUME_NAME "ScreenPlay")
set(CPACK_SYSTEM_NAME "OSX")
set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/ScreenPlay/assets/icons/app.ico")

set(CPACK_COMPONENTS_GROUPING IGNORE)
set(CPACK_PACKAGE_NAME "ScreenPlay")
set(CPACK_PACKAGE_FILE_NAME "ScreenPlay-Installer")
set(CPACK_PACKAGE_VENDOR "Elias Steurer")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_DESCRIPTION}")
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_BINARY_DIR}")

include(CPack)

# Install all files from /bin
install(
    DIRECTORY "${CMAKE_BINARY_DIR}/bin/"
    COMPONENT ScreenPlay
    DESTINATION "./")