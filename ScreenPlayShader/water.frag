uniform sampler2D qt_Texture0;
varying vec4 qt_TexCoord0;

uniform float reflectionOffset = 0; // allows player to control reflection position
uniform float reflectionBlur = 0; // works only if projec's driver is set to GLES3, more information here https://docs.godotengine.org/ru/stable/tutorials/shading/screen-reading_shaders.html
uniform float calculatedOffset = 0; // this is controlled by script, it takes into account camera position and water object position, that way reflection stays in the same place when camera is moving
uniform float calculatedAspect = 1; // is controlled by script, ensures that noise is not affected by object scale
uniform sampler2D noiseTexture;
uniform float offsetStrength;
uniform float maxOffset;

uniform vec2 distortionScale = vec2(0.3, 0.3);
uniform vec2 distortionSpeed = vec2(0.01, 0.02);

uniform float waveSmoothing = .01;

uniform float mainWaveSpeed = 2.5;
uniform float mainWaveFrequency = 20;
uniform float mainWaveAmplitude = 0.005;

uniform float secondWaveSpeed = 2.5;
uniform float secondWaveFrequency = 20;
uniform float secondWaveAmplitude = 0.015;

uniform float thirdWaveSpeed = 2.5;
uniform float thirdWaveFrequency = 20;
uniform float thirdWaveAmplitude = 0.01;

uniform float squashing = 1.0;

uniform vec4 shorelineColor = vec4(1.0, 1.0, 1.0, 1.0); //: hint_color = vec4(1.);
uniform float shorelineSize  = 0.0025; //: hint_range(0., 0.1) = 0.0025;
uniform float shorelineFoamSize : hint_range(0., 0.1) = 0.0025;
uniform float foamSpeed;
uniform vec2 foamScale;

uniform float time;


void main(void)
{
    gl_FragColor = texture2D(qt_Texture0, qt_TexCoord0.st);
}
