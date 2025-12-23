# FixSentryCrashpad.cmake
# Workaround for vcpkg sentry-native crashpad_wer target missing IMPORTED_IMPLIB
# 
# crashpad_wer.dll is a Windows Error Reporting (WER) module that handles 
# "fast-fail" crashes bypassing SEH. It's registered with Windows during 
# sentry_init() and loaded dynamically by WER - NOT linked against.
# See: https://github.com/getsentry/sentry-native/issues/875
#
# The vcpkg cmake config incorrectly declares it as SHARED IMPORTED without
# an import library, causing CMake to error. We fix this by changing it to
# an INTERFACE target since we don't actually link against it.

function(fix_sentry_crashpad_wer)
    if(TARGET sentry_crashpad::crashpad_wer)
        # Get the DLL location before we modify the target
        get_target_property(_dll_location sentry_crashpad::crashpad_wer IMPORTED_LOCATION)
        if(NOT _dll_location)
            get_target_property(_dll_location sentry_crashpad::crashpad_wer IMPORTED_LOCATION_RELEASE)
        endif()
        
        # crashpad_wer is a WER runtime module, not a linkable library.
        # Set empty IMPORTED_IMPLIB to satisfy CMake's validation.
        set_target_properties(sentry_crashpad::crashpad_wer PROPERTIES
            IMPORTED_IMPLIB "${_dll_location}"
            IMPORTED_IMPLIB_DEBUG "${_dll_location}"
            IMPORTED_IMPLIB_RELEASE "${_dll_location}"
        )
        message(STATUS "Fixed sentry_crashpad::crashpad_wer IMPORTED_IMPLIB (WER module workaround)")
    endif()
endfunction()
