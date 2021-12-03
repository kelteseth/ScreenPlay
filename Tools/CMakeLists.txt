project(Tools LANGUAGES CXX)

file(GLOB PYTHON *.py)

set(FILES
    # cmake-format: sortable
    Installer/package.xml
    Installer/installscript.qs)

add_custom_target(
    ${PROJECT_NAME}
    SOURCES ${FILES} ${PYTHON}
    COMMENT "Dummy target to list these files in the IDE")
