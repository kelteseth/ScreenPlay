import QtQuick
import QtQuick.Controls.Material
import QtQuick.Effects
/*!
   \qmltype ColorImage
   \inqmlmodule Common
   \inherits Image
   \brief An Image whose color can be specified.
   The ColorImage component extends the Image component by the color property.
*/
Image {
    id: root
    /*!
        \qmlproperty color ColorImage::color
        The color of the image.
    */
    property color color: Material.iconColor
    layer {
        enabled: true
        effect: MultiEffect {
            colorizationColor: root.color
            colorization: 1.0
        }
    }
}
