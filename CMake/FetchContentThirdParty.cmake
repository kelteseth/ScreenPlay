cmake_minimum_required(VERSION 3.16.0)

include(FetchContent)

set(THIRD_PARTY_PATH "${CMAKE_SOURCE_DIR}/ThirdParty/")

FetchContent_Populate(
  QArchive
  GIT_REPOSITORY https://github.com/antony-jr/QArchive.git
  GIT_TAG        2d05e652ad9a2bff8c87962d5525e2b3c4d7351b
  # Workaround because:
  # 1. QtCreator cannot handle QML_ELEMENT stuff when it is in bin folder
  # https://bugreports.qt.io/browse/QTCREATORBUG-27083
  SOURCE_DIR ${THIRD_PARTY_PATH}/QArchive
)


FetchContent_Populate(
  qml-plausible
  GIT_REPOSITORY https://gitlab.com/kelteseth/qml-plausible.git
  GIT_TAG        00398446c7a2882a11d34c007a1ed8205c72e123
  # Workaround because:
  # 1. QtCreator cannot handle QML_ELEMENT stuff when it is in bin folder
  # https://bugreports.qt.io/browse/QTCREATORBUG-27083
  SOURCE_DIR ${THIRD_PARTY_PATH}/qml-plausible
)
