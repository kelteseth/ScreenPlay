cmake_minimum_required(VERSION 3.16.0)

include(FetchContent)

set(THIRD_PARTY_PATH "${CMAKE_SOURCE_DIR}/ThirdParty/")

FetchContent_Populate(
  QArchive
  GIT_REPOSITORY https://github.com/antony-jr/QArchive.git
  GIT_TAG        cb9c6ea9300f5f37d581ba4f039cfdb7bcf3c9e6
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
