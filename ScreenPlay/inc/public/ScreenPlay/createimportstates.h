// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include <QObject>
#include <QQmlEngine>

namespace ScreenPlay {

class Import : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_CLASSINFO("RegisterEnumClassesUnscoped", "false")
public:
    enum class State {
        Idle,
        Started,
        AnalyseVideo,
        AnalyseVideoFinished,
        AnalyseVideoError,
        AnalyseVideoHasNoVideoStreamError,
        ConvertingPreviewVideo,
        ConvertingPreviewVideoFinished,
        ConvertingPreviewVideoError,
        ConvertingPreviewGif,
        ConvertingPreviewGifFinished, // 10
        ConvertingPreviewGifError,
        ConvertingPreviewImage,
        ConvertingPreviewImageFinished,
        ConvertingPreviewImageError,
        ConvertingPreviewImageThumbnail,
        ConvertingPreviewImageThumbnailFinished,
        ConvertingPreviewImageThumbnailError,
        ConvertingAudio,
        ConvertingAudioFinished,
        ConvertingAudioError, // 20
        ConvertingVideo,
        ConvertingVideoFinished,
        ConvertingVideoError,
        CopyFiles,
        CopyFilesFinished,
        CopyFilesError,
        CreateProjectFile,
        CreateProjectFileFinished,
        CreateProjectFileError,
        AbortCleanupError, // 30
        CreateTmpFolderError,
        Finished,
        Failed,
    };
    Q_ENUM(State)
};
}
