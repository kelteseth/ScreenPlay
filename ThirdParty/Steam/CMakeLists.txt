# Steam API installation

if(${SCREENPLAY_STEAM})
    if(WIN32)
        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/win64/steam_api64.dll DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
    elseif(APPLE)
        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/osx/libsteam_api.dylib DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
    elseif(UNIX)
        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/linux64/libsteam_api.so DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
    endif()
endif()
