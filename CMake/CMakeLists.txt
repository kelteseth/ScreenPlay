project(CMake)

set(FILES
    CMakeVariables.h.in
    GetProjectVersion.cmake
    GenerateCMakeVariableHeader.cmake
    CopyRecursive.cmake
    QtDeploy.cmake
    CreateIFWInstaller.cmake)

add_custom_target(
    ${PROJECT_NAME}
    SOURCES ${FILES}
    COMMENT "Dummy target to list these files in the IDE")
