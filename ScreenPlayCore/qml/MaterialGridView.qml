import QtQuick

/*!
    \qmltype MaterialGridView
    \brief A GridView with consistent flick behavior and removal animations.

    MaterialGridView provides default flickDeceleration and maximumFlickVelocity
    values to ensure smooth and consistent scrolling experience throughout ScreenPlay.
    It also includes shared remove and displaced transitions so items shrink away
    smoothly when removed and remaining items reflow into place.
*/
GridView {
    id: root

    flickDeceleration: 0.001
    maximumFlickVelocity: 4000

    remove: Transition {
        ParallelAnimation {
            NumberAnimation { property: "scale"; to: 0; duration: 300; easing.type: Easing.InBack }
            NumberAnimation { property: "opacity"; to: 0; duration: 300; easing.type: Easing.InQuad }
        }
    }

    displaced: Transition {
        NumberAnimation { properties: "x,y"; duration: 400; easing.type: Easing.OutCubic }
    }
}
