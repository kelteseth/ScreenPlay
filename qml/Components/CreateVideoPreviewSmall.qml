import QtQuick 2.0
import QtAV 1.07

Rectangle {
    id: createVideoPreviewSmall
    property url source


    Video {
        id: previewVideo
        anchors.fill: parent
        z: 99
        source: createVideoPreviewSmall.source
        onSourceChanged: {
            print(previewVideo.error + previewVideo.errorString + previewVideo.source)
            //previewVideo.source = createVideoPreviewSmall.source;
            previewVideo.play()
        }
        onPlaying: {
            print("playing")
        }


        onErrorChanged: {
            print(previewVideo.error + previewVideo.errorString)
        }

        //videoCodecPriority: ["CUDA", "D3D11", "DXVA", "VAAPI", "FFmpeg"]
    }
}
