# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
project(Tools LANGUAGES CXX)

file(GLOB PYTHON *.py)

set(FILES Installer/installscript.qs Installer/package.xml)

add_custom_target(
    ${PROJECT_NAME}
    SOURCES ${FILES} ${PYTHON}
    COMMENT "Dummy target to list these files in the IDE")

set(GODOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/Godot")

if(WIN32)
    # Todo: Add Linux and Mac support
    configure_file(${GODOT_PATH}/${GODOT_EDITOR_NAME} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ COPYONLY)
endif()
