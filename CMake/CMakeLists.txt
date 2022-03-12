project(CMake)

set(FILES
    # cmake-format: sortable
    CopyRecursive.cmake
    CreateIFWInstaller.cmake
    FetchContentThirdParty.cmake
    QtUpdateTranslations.cmake)

add_custom_target(
    ${PROJECT_NAME}
    SOURCES ${FILES}
    COMMENT "Dummy target to list these files in the IDE")
