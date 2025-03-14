message(STATUS "[IFW INSTALLER ENABLED]: Configuring. This can take some time...")

set(CPACK_COMPONENTS_GROUPING IGNORE)
set(CPACK_PACKAGE_NAME "ScreenPlay")
set(CPACK_PACKAGE_FILE_NAME "ScreenPlay-Installer")
set(CPACK_PACKAGE_VENDOR "Elias Steurer")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_DESCRIPTION}")
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_BINARY_DIR}")

# Ensures that contents written to the CPack configuration files is escaped properly.
set(CPACK_VERBATIM_VARIABLES TRUE)
set(CPACK_GENERATOR "IFW")

set(CPACK_IFW_PACKAGE_NAME "ScreenPlay")
set(CPACK_IFW_ROOT ${SCREENPLAY_IFW_ROOT})
set(CPACK_IFW_TARGET_DIRECTORY "@HomeDir@/Apps/ScreenPlay")
set(CPACK_IFW_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/ScreenPlay/assets/icons/app.ico")
set(CPACK_IFW_PACKAGE_WINDOW_ICON "${CMAKE_SOURCE_DIR}/ScreenPlay/assets/icons/app.ico")
set(CPACK_IFW_PACKAGE_CONTROL_SCRIPT "${CMAKE_SOURCE_DIR}/Tools/Installer/installscript.qs")

set(CPACK_IFW_PACKAGE_GROUP ScreenPlay)
set(CPACK_IFW_VERBOSE ON)

# Override weird size defaults on 21:9 monitors
set(CPACK_IFW_PACKAGE_WIZARD_DEFAULT_WIDTH 800)
set(CPACK_IFW_PACKAGE_WIZARD_DEFAULT_HEIGHT 506)

set(CPACK_IFW_PACKAGE_RUN_PROGRAM "@TargetDir@/ScreenPlay")
set(CPACK_IFW_PACKAGE_RUN_PROGRAM_DESCRIPTION "Start ScreenPlay")
set(CPACK_IFW_ARCHIVE_FORMAT "tar.bz2")

include(CPack)
include(CPackIFW)

# Install all files from /bin
install(
    DIRECTORY "${CMAKE_BINARY_DIR}/bin/"
    COMPONENT ScreenPlay
    DESTINATION "./")

cpack_add_component(
    ScreenPlay
    DISPLAY_NAME "ScreenPlay"
    DESCRIPTION "This installs ScreenPlay.")

cpack_ifw_configure_component(
    ScreenPlayApp FORCED_INSTALLATION
    NAME "ScreenPlay"
    VERSION ${PROJECT_VERSION} # Version of component
    DESCRIPTION "Welcome to the ScreenPlay installer."
    CHECKABLE FALSE)
