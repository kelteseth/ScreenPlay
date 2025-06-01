function(get_build_info)
    # Find Git executable
    find_package(Git QUIET)
    if(NOT GIT_FOUND)
        message(WARNING "Git not found. Git-related variables will be empty.")
    endif()

    # Set platform-specific date command
    if(WIN32)
        set(DATE_COMMAND "CMD")
        set(DATE_ARG "/c date /t")
    else()
        set(DATE_COMMAND "date")
        set(DATE_ARG "")
    endif()

    # Get build date
    execute_process(
        COMMAND ${DATE_COMMAND} ${DATE_ARG}
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE BUILD_DATE
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET)

    # Get git branch name
    if(GIT_FOUND)
        execute_process(
            COMMAND "${GIT_EXECUTABLE}" rev-parse --abbrev-ref HEAD
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            OUTPUT_VARIABLE GIT_BRANCH_NAME
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET)
    else()
        set(GIT_BRANCH_NAME "unknown")
    endif()

    # Get git commit hash
    if(GIT_FOUND)
        execute_process(
            COMMAND "${GIT_EXECUTABLE}" rev-parse --short HEAD
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            OUTPUT_VARIABLE GIT_COMMIT_HASH
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET)
    else()
        set(GIT_COMMIT_HASH "unknown")
    endif()

    # Set variables in parent scope
    set(BUILD_DATE "${BUILD_DATE}" PARENT_SCOPE)
    set(GIT_BRANCH_NAME "${GIT_BRANCH_NAME}" PARENT_SCOPE)
    set(GIT_COMMIT_HASH "${GIT_COMMIT_HASH}" PARENT_SCOPE)
endfunction()