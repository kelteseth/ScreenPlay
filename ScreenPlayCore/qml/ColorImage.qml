import QtQuick
import QtQuick.Controls.Material
import ScreenPlayCore

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
        effect: ShaderEffect {
            property color overlayColor: root.color
            // Must import import ScreenPlayCore?
            fragmentShader: "/qml/shaders/color_image.frag.qsb"
        }
    }
}
