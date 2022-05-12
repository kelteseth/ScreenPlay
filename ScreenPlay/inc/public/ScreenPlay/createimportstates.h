#pragma once

#include <QObject>

namespace ScreenPlay {
/*!
    \namespace ScreenPlay::ImportVideoState
    \inmodule ScreenPlay
    \brief Global enum for ImportVideoState.
*/
namespace ImportVideoState {
    Q_NAMESPACE

    enum class ImportVideoState {
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
        ConvertingPreviewGifFinished, //10
        ConvertingPreviewGifError,
        ConvertingPreviewImage,
        ConvertingPreviewImageFinished,
        ConvertingPreviewImageError,
        ConvertingPreviewImageThumbnail,
        ConvertingPreviewImageThumbnailFinished,
        ConvertingPreviewImageThumbnailError,
        ConvertingAudio,
        ConvertingAudioFinished,
        ConvertingAudioError, //20
        ConvertingVideo,
        ConvertingVideoFinished,
        ConvertingVideoError,
        CopyFiles,
        CopyFilesFinished,
        CopyFilesError,
        CreateProjectFile,
        CreateProjectFileFinished,
        CreateProjectFileError,
        AbortCleanupError, //30
        CreateTmpFolderError,
        Finished,
        Failed,
    };
    Q_ENUM_NS(ImportVideoState)
}
}
