find_package(Qt6 REQUIRED COMPONENTS Core)

# Initialize global properties
set_property(GLOBAL PROPERTY QT_DEPLOY_TARGETS "")
set_property(GLOBAL PROPERTY QT_DEPLOY_MAIN_TARGET "")

# Function to register main target for deployment
function(register_qt_main_deployment TARGET_NAME)
    set_property(GLOBAL PROPERTY QT_DEPLOY_MAIN_TARGET ${TARGET_NAME})
endfunction()

# Function to register additional executables for deployment
function(register_qt_additional_deployment TARGET_NAME)
    get_property(targets GLOBAL PROPERTY QT_DEPLOY_TARGETS)
    list(APPEND targets ${TARGET_NAME})
    set_property(GLOBAL PROPERTY QT_DEPLOY_TARGETS "${targets}")
    
    if(APPLE)
        get_property(main_target GLOBAL PROPERTY QT_DEPLOY_MAIN_TARGET)
        if(NOT main_target)
            message(FATAL_ERROR "No main target registered. Call register_qt_main_deployment first.")
        endif()
        
        # Important: Set output directories to be within the app bundle
        set_target_properties(${TARGET_NAME} PROPERTIES
            MACOSX_BUNDLE FALSE
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${main_target}.app/Contents/MacOS"
            LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${main_target}.app/Contents/Frameworks"
        )
        
        # Install directly into the app bundle
        install(TARGETS ${TARGET_NAME}
            RUNTIME DESTINATION "${main_target}.app/Contents/MacOS"
            LIBRARY DESTINATION "${main_target}.app/Contents/Frameworks"
        )
    else()
        install(TARGETS ${TARGET_NAME}
            BUNDLE DESTINATION .
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        )
    endif()
endfunction()

# Function to generate single deployment script
function(generate_qt_deployment_script)
    get_property(main_target GLOBAL PROPERTY QT_DEPLOY_MAIN_TARGET)
    get_property(additional_targets GLOBAL PROPERTY QT_DEPLOY_TARGETS)
    
    if(NOT main_target)
        message(FATAL_ERROR "No main target registered for deployment")
        return()
    endif()
    
    # Prepare paths based on platform
    if(APPLE)
        set(main_exe "${CMAKE_INSTALL_PREFIX}/${main_target}.app")
        set(additional_exes "")
        foreach(target ${additional_targets})
            list(APPEND additional_exes "${main_exe}/Contents/MacOS/$<TARGET_FILE_NAME:${target}>")
        endforeach()
    else()
        set(main_exe "$<TARGET_FILE:${main_target}>")
        set(additional_exes "")
        foreach(target ${additional_targets})
            list(APPEND additional_exes "$<TARGET_FILE:${target}>")
        endforeach()
    endif()
    
    set(deploy_script "${CMAKE_BINARY_DIR}/qt_deploy_all.cmake")
    
    # Generate deployment script content
    set(script_content "
    include(\"${QT_DEPLOY_SUPPORT}\")
    
    # Deploy QML imports for main target
    qt_deploy_qml_imports(
        TARGET ${main_target}
        PLUGINS_FOUND main_plugins_found
        QML_DIR \"qml\"
    )
    
    # Deploy QML imports for additional targets
    set(all_plugins \${main_plugins_found})")
    
    foreach(target ${additional_targets})
        string(APPEND script_content "
        qt_deploy_qml_imports(
            TARGET ${target}
            PLUGINS_FOUND ${target}_plugins_found
            QML_DIR \"qml\"
        )
        list(APPEND all_plugins \${${target}_plugins_found})")
    endforeach()
    
    # Add runtime dependencies deployment
    if(APPLE)
        string(APPEND script_content "
        
        # Deploy runtime dependencies including all QML plugins
        qt_deploy_runtime_dependencies(
            EXECUTABLE \"${main_exe}\"
            ADDITIONAL_EXECUTABLES \"${additional_exes}\"
            ADDITIONAL_MODULES \${all_plugins}
            GENERATE_QT_CONF
            BIN_DIR Contents/MacOS
            PLUGINS_DIR Contents/PlugIns
            QML_DIR Contents/Resources/qml
        )")
    else()
        string(APPEND script_content "
        
        # Deploy runtime dependencies including all QML plugins
        qt_deploy_runtime_dependencies(
            EXECUTABLE \"${main_exe}\"
            ADDITIONAL_EXECUTABLES \"${additional_exes}\"
            ADDITIONAL_MODULES \${all_plugins}
            GENERATE_QT_CONF
            BIN_DIR bin
            PLUGINS_DIR plugins
            QML_DIR qml
        )")
    endif()
    
    file(GENERATE OUTPUT ${deploy_script} CONTENT "${script_content}")
    install(SCRIPT ${deploy_script})
endfunction()