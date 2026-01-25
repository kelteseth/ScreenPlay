import QtQuick

/*!
    \qmltype MaterialGridView
    \brief A GridView with consistent flick behavior across the application.

    MaterialGridView provides default flickDeceleration and maximumFlickVelocity
    values to ensure smooth and consistent scrolling experience throughout ScreenPlay.
*/
GridView {
    id: root

    flickDeceleration: 0.001
    maximumFlickVelocity: 4000
}
