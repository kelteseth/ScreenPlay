
find_program(LUPDATE_EXECUTABLE lupdate)
find_program(LRELEASE_EXECUTABLE lrelease)

# Updates all ts files and generates .qm
# Absolute paths are needed!
# ibh_qt_update_translations("${CMAKE_CURRENT_SOURCE_DIR}/Src" "${CMAKE_CURRENT_SOURCE_DIR}/Translations")
# The second argument (array) __must__ be passed as string!
function(ibh_qt_update_translations SOURCE_PATH TS_FILES)
    message(STATUS "Update Translation: ${SOURCE_PATH}")

    foreach(_ts_file ${TS_FILES})
        execute_process(COMMAND ${LUPDATE_EXECUTABLE} -noobsolete -recursive ${SOURCE_PATH} -ts ${_ts_file} OUTPUT_QUIET)
        execute_process(COMMAND ${LRELEASE_EXECUTABLE} ${_ts_file} OUTPUT_QUIET)
    endforeach()

endfunction()
