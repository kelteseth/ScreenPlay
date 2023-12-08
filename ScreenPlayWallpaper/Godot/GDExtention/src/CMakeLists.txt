# SPDX-License-Identifier: Unlicense

target_sources(
    ${PROJECT_NAME}
    PRIVATE ScreenPlayGodotWallpaper.h
            ScreenPlayGodotWallpaper.cpp
            WindowsPipe.h
            WindowsPipe.cpp
            RegisterExtension.cpp)
find_package(Catch2 CONFIG REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE "src")

# Test app. Start WindowsPipeTestServer.py!
if(${SCREENPLAY_TESTS})
    add_executable(WindowsPipeTest WindowsPipe.cpp WindowsPipe.h WindowsPipeTest.cpp)
    target_link_libraries(WindowsPipeTest PRIVATE Catch2::Catch2 Catch2::Catch2WithMain)
    target_include_directories(WindowsPipeTest PRIVATE "src")
endif()
