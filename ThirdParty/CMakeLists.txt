include(FetchContent)

FetchContent_Populate(
  QArchive
  GIT_REPOSITORY https://github.com/antony-jr/QArchive.git
  GIT_TAG        e587f30507c0e6d92f79a2dc1a6aa7ebb1f8e679
  # Workaround because:
  # 1. QtCreator cannot handle QML_ELEMENT stuff when it is in bin folder
  # https://bugreports.qt.io/browse/QTCREATORBUG-27083
  SOURCE_DIR ${THIRD_PARTY_PATH}/QArchive
)
#lol
FetchContent_Populate(
  qml-plausible
  GIT_REPOSITORY https://gitlab.com/kelteseth/qml-plausible.git
  GIT_TAG        5069ba3bf25663ea06be8b94c398d6c61058d4d5
  # Workaround because:
  # 1. QtCreator cannot handle QML_ELEMENT stuff when it is in bin folder
  # https://bugreports.qt.io/browse/QTCREATORBUG-27083
  SOURCE_DIR ${THIRD_PARTY_PATH}/qml-plausible
)

add_subdirectory(qml-plausible)
add_subdirectory(QArchive)