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
        
        # Get the bundle name from the main target
        get_target_property(bundle_name ${main_target} OUTPUT_NAME)
        if(NOT bundle_name)
            get_target_property(bundle_name ${main_target} MACOSX_BUNDLE_BUNDLE_NAME)
        endif()
        if(NOT bundle_name)
            set(bundle_name ${main_target})
        endif()
        
        # Set output directories using the bundle name
        set_target_properties(${TARGET_NAME} PROPERTIES
            MACOSX_BUNDLE FALSE
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${bundle_name}.app/Contents/MacOS"
            LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${bundle_name}.app/Contents/Frameworks"
        )
        
        # Install using the bundle name
        install(TARGETS ${TARGET_NAME}
            RUNTIME DESTINATION "${bundle_name}.app/Contents/MacOS"
            LIBRARY DESTINATION "${bundle_name}.app/Contents/Frameworks"
        )
    else()
        install(TARGETS ${TARGET_NAME}
            BUNDLE DESTINATION .
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        )
    endif()
endfunction()

function(generate_qt_deployment_script)
    get_property(main_target GLOBAL PROPERTY QT_DEPLOY_MAIN_TARGET)
    get_property(additional_targets GLOBAL PROPERTY QT_DEPLOY_TARGETS)
    
    if(NOT main_target)
        message(FATAL_ERROR "No main target registered for deployment")
        return()
    endif()
    
    # Get actual bundle name from target properties
    get_target_property(bundle_name ${main_target} OUTPUT_NAME)
    if(NOT bundle_name)
        get_target_property(bundle_name ${main_target} MACOSX_BUNDLE_BUNDLE_NAME)
    endif()
    if(NOT bundle_name)
        set(bundle_name ${main_target})
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
    
    if(APPLE)
        set(bundle_path "${CMAKE_INSTALL_PREFIX}/${bundle_name}.app")
        
        # Create list of additional executables
        set(additional_exes "")
        foreach(target ${additional_targets})
            list(APPEND additional_exes "${bundle_name}.app/Contents/MacOS/$<TARGET_FILE_NAME:${target}>")
        endforeach()
        
        string(APPEND script_content "
        
        # Deploy runtime dependencies including all QML plugins
        qt_deploy_runtime_dependencies(
            EXECUTABLE \"${bundle_name}.app\"
            ADDITIONAL_EXECUTABLES \"${additional_exes}\"
            ADDITIONAL_MODULES \${all_plugins}
            GENERATE_QT_CONF
            BIN_DIR Contents/MacOS
            PLUGINS_DIR Contents/PlugIns
            QML_DIR Contents/Resources/qml
        )")
    else()
        # For non-Apple platforms
        string(APPEND script_content "
        
        # Deploy runtime dependencies including all QML plugins
        qt_deploy_runtime_dependencies(
            EXECUTABLE \"\${CMAKE_INSTALL_PREFIX}/bin/$<TARGET_FILE_NAME:${main_target}>\"
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