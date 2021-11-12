find_program(LUPDATE_EXECUTABLE lupdate)
find_program(LRELEASE_EXECUTABLE lrelease)

# Updates all ts files and generates .qm
# Absolute paths are needed!
# qt_update_translations("${CMAKE_CURRENT_SOURCE_DIR}/qml" "${L10N_LIST}")
# The second argument (list) __must__ be passed as string!
function(qt_update_translations SOURCE_PATH TS_FILES)

    foreach(_ts_file ${TS_FILES})
        message(STATUS "Update Translation: ${_ts_file}")
        execute_process(COMMAND ${LUPDATE_EXECUTABLE} -noobsolete -locations none -recursive ${SOURCE_PATH} -ts ${_ts_file} OUTPUT_QUIET)
        execute_process(COMMAND ${LRELEASE_EXECUTABLE} ${_ts_file} OUTPUT_QUIET)
    endforeach()

endfunction()
