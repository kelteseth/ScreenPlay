# SPDX-License-Identifier: Unlicense

target_sources( ${PROJECT_NAME}
    PRIVATE
        ScreenPlayWallpaper.h
        ScreenPlayWallpaper.cpp
        GDExtensionTemplate.h
        GDExtensionTemplate.cpp
        RegisterExtension.cpp
)

target_include_directories( ${PROJECT_NAME}
    PRIVATE
        "src"
)
