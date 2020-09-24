import QtQuick 2.12
import QtQuick.Controls 2.12

ShaderEffect {
    id: root
    property real speed: 1

    readonly property vector3d defaultResolution: Qt.vector3d(
                                                      root.width, root.height,
                                                      root.width / root.height)
    function calcResolution(channel) {
        if (channel) {
            return Qt.vector3d(channel.width, channel.height,
                               channel.width / channel.height)
        } else {
            return defaultResolution
        }
    }

    // based on shadertoy default variables
    readonly property vector3d iResolution: defaultResolution
    property real iTime: 0
    property real iTimeDelta: 100
    property int iFrame: 10
    property real iFrameRate
    property double shaderSpeed: 1.0
    property vector4d iMouse

    //only Image or ShaderEffectSource
    property var iChannel0: ich0
    property var iChannel1: ich1
    property var iChannel2: ich2
    property var iChannel3: ich3

    property var iChannelTime: [0, 1, 2, 3]
    property var iChannelResolution: [calcResolution(iChannel0), calcResolution(
            iChannel1), calcResolution(iChannel2), calcResolution(iChannel3)]
    property vector4d iDate
    property real iSampleRate: 44100

    Image {
        id: ich0
        visible: false
    }
    Image {
        id: ich1
        visible: false
    }
    Image {
        id: ich2
        visible: false
    }
    Image {
        id: ich3
        visible: false
    }

    property bool hoverEnabled: false
    property bool running: true
    function restart() {
        root.iTime = 0
        running = true
        timer1.restart()
    }
    Timer {
        id: timer1
        running: root.running
        triggeredOnStart: true
        interval: 16
        repeat: true
        onTriggered: {
            root.iTime += 0.016 * speed
        }
    }
    Timer {
        running: root.running
        interval: 1000
        property date currentDate: new Date()
        onTriggered: {
            currentDate = new Date()
            root.iDate.x = currentDate.getFullYear()
            root.iDate.y = currentDate.getMonth()
            root.iDate.z = currentDate.getDay()
            root.iDate.w = currentDate.getSeconds()
        }
    }

    readonly property string gles2Ver: "
#define texture texture2D
precision mediump float;"

    readonly property string gles3Ver: "#version 300 es
#define varying in
#define gl_FragColor fragColor
precision mediump float;
out vec4 fragColor;"

    readonly property string gl3Ver: "
#version 150
#define varying in
#define gl_FragColor fragColor
#define lowp
#define mediump
#define highp
out vec4 fragColor;"

    readonly property string gl3Ver_igpu: "
#version 130
#define varying in
#define gl_FragColor fragColor
#define lowp
#define mediump
#define highp
out vec4 fragColor;"

    readonly property string gl2Ver: "
#version 110
#define texture texture2D"

    property string versionString: (GraphicsInfo.majorVersion === 3
                                    || GraphicsInfo.majorVersion === 4) ? gl3Ver : gl2Ver

    vertexShader: "
uniform mat4 qt_Matrix;
attribute vec4 qt_Vertex;
attribute vec2 qt_MultiTexCoord0;
varying vec2 qt_TexCoord0;
varying vec4 vertex;
void main() {
vertex = qt_Vertex;
gl_Position = qt_Matrix * vertex;
qt_TexCoord0 = qt_MultiTexCoord0;
}"

    readonly property string forwardString: versionString + "
varying vec2 qt_TexCoord0;
varying vec4 vertex;
uniform lowp   float qt_Opacity;
uniform vec3   iResolution;
uniform float  iTime;
uniform float  iTimeDelta;
uniform int    iFrame;
uniform float  iFrameRate;
uniform float  iChannelTime[4];
uniform vec3   iChannelResolution[4];
uniform vec4   iMouse;
uniform vec4    iDate;
uniform float   iSampleRate;
uniform sampler2D   iChannel0;
uniform sampler2D   iChannel1;
uniform sampler2D   iChannel2;
uniform sampler2D   iChannel3;"

    readonly property string startCode: "
void main(void)
{
mainImage(gl_FragColor, vec2(vertex.x, iResolution.y - vertex.y));
}"

    readonly property string defaultPixelShader: "
void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
fragColor = vec4(fragCoord, fragCoord.x, fragCoord.y);
}"

    property bool runShader: true
    property string pixelShader
    onPixelShaderChanged: root.fragmentShader = forwardString + pixelShader + startCode
}
