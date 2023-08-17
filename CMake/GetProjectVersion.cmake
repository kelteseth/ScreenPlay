# Function: get_project_version
#
# Description:
#   Fetches the project version from the latest Git tag. If Git is not found or
#   the current directory is not a Git repository, it defaults to "0.0.0".
#
# Parameters:
#   - VERSION_VAR: The name of the variable in which the fetched or default version will be stored.
#
# Example Usage:
#   get_project_version(PROJECT_VERSION)
#   message(STATUS "Version: ${PROJECT_VERSION}")
#
function(get_project_version VERSION_VAR)
    find_package(Git)
    if(GIT_FOUND)
        execute_process(
            COMMAND ${GIT_EXECUTABLE} describe --tags --always
            OUTPUT_VARIABLE GIT_VERSION
            OUTPUT_STRIP_TRAILING_WHITESPACE
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        )

        message(STATUS "Parsing git tag: ${GIT_VERSION}")
        string(REPLACE "V" "" STRIPPED_VERSION "${GIT_VERSION}") # Remove the 'V' prefix
        string(REPLACE "-" ";" VERSION_LIST ${STRIPPED_VERSION})
        list(GET VERSION_LIST 0 VERSION_STRING)

        set(${VERSION_VAR} ${VERSION_STRING} PARENT_SCOPE)
    else()
        set(${VERSION_VAR} "0.0.0" PARENT_SCOPE)
    endif()
endfunction()
