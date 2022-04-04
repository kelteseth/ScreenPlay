project(Tools LANGUAGES CXX)

file(GLOB PYTHON *.py)

set(FILES 
          Installer/installscript.qs Installer/package.xml)

add_custom_target(
    ${PROJECT_NAME}
    SOURCES ${FILES} ${PYTHON}
    COMMENT "Dummy target to list these files in the IDE")
