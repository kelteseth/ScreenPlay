# SPDX-License-Identifier: Unlicense

target_sources(${PROJECT_NAME}
    PRIVATE
    ScreenPlayGodotWallpaper.h
    ScreenPlayGodotWallpaper.cpp
    RegisterExtension.cpp
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
    "src"
)
