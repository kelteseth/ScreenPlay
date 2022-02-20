cmake_minimum_required(VERSION 3.16.0)

include(FetchContent)

FetchContent_Declare(
    qml-plausible
    GIT_REPOSITORY https://gitlab.com/kelteseth/qml-plausible.git
    GIT_TAG 00398446c7a2882a11d34c007a1ed8205c72e123)

FetchContent_MakeAvailable(qml-plausible)
