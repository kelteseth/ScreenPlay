include(FetchContent)

FetchContent_Populate(
    QArchive
    GIT_REPOSITORY https://github.com/antony-jr/QArchive.git
    GIT_TAG e587f30507c0e6d92f79a2dc1a6aa7ebb1f8e679
    # Workaround because: 1. QtCreator cannot handle QML_ELEMENT stuff when it is in bin folder
    # https://bugreports.qt.io/browse/QTCREATORBUG-27083
    SOURCE_DIR ${THIRD_PARTY_PATH}/QArchive)

FetchContent_Populate(
    qcoro
    GIT_REPOSITORY https://github.com/danvratil/qcoro.git
    GIT_TAG 3a6efdb
    # Workaround because: 1. QtCreator cannot handle QML_ELEMENT stuff when it is in bin folder
    # https://bugreports.qt.io/browse/QTCREATORBUG-27083
    SOURCE_DIR ${THIRD_PARTY_PATH}/qqcoro)

add_subdirectory(QArchive)
add_subdirectory(qqcoro)

qcoro_enable_coroutines()

if(UNIX AND NOT APPLE)
    FetchContent_Populate(
        qt-layer-shell
        GIT_REPOSITORY https://github.com/KDE/layer-shell-qt.git
        GIT_TAG 721c0ae334554eb2396a2d4d3358f896b8c77412
        # Workaround because: 1. QtCreator cannot handle QML_ELEMENT stuff when it is in bin folder
        # https://bugreports.qt.io/browse/QTCREATORBUG-27083
        SOURCE_DIR ${THIRD_PARTY_PATH}/qt-layer-shell)
    add_subdirectory(qt-layer-shell)
endif()
