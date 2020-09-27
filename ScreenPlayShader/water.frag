
#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D qt_Texture0;
varying vec4 qt_TexCoord0;

//uniform float reflectionOffset; // allows player to control reflection position
//uniform float reflectionBlur ; // works only if projec's driver is set to GLES3, more information here https://docs.godotengine.org/ru/stable/tutorials/shading/screen-reading_shaders.html
//uniform float calculatedOffset ; // this is controlled by script, it takes into account camera position and water object position, that way reflection stays in the same place when camera is moving
//uniform float calculatedAspect ; // is controlled by script, ensures that noise is not affected by object scale
//uniform sampler2D noiseTexture;
//uniform float offsetStrength;
//uniform float maxOffset;

//uniform vec2 distortionScale;
//uniform vec2 distortionSpeed;

//uniform float waveSmoothing;

//uniform float mainWaveSpeed ;
//uniform float mainWaveFrequency ;
//uniform float mainWaveAmplitude;

//uniform float secondWaveSpeed ;
//uniform float secondWaveFrequency ;
//uniform float secondWaveAmplitude ;

//uniform float thirdWaveSpeed ;
//uniform float thirdWaveFrequency ;
//uniform float thirdWaveAmplitude ;

//uniform float squashing ;

//uniform vec4 shorelineColor; //: hint_color = vec4(1.);
//uniform float shorelineSize;                  //: hint_range(0., 0.1) = 0.0025;
//uniform float shorelineFoamSize  ;              // : hint_range(0., 0.1)
//uniform float foamSpeed;
//uniform vec2 foamScale;

uniform float time;


void main(void)
{
        vec2 uv = fragCoord.xy / iResolution.xy;
    vec4 texture_color = vec4(0.192156862745098, 0.6627450980392157, 0.9333333333333333, 1.0);

      vec4 k = vec4(time)*0.8;
       k.xy = uv * 7.0;
      float val1 = length(0.5-fract(k.xyw*=mat3(vec3(-2.0,-1.0,0.0), vec3(3.0,-1.0,1.0), vec3(1.0,-1.0,-1.0))*0.5));
      float val2 = length(0.5-fract(k.xyw*=mat3(vec3(-2.0,-1.0,0.0), vec3(3.0,-1.0,1.0), vec3(1.0,-1.0,-1.0))*0.2));
      float val3 = length(0.5-fract(k.xyw*=mat3(vec3(-2.0,-1.0,0.0), vec3(3.0,-1.0,1.0), vec3(1.0,-1.0,-1.0))*0.5));
      vec4 color = vec4 ( pow(min(min(val1,val2),val3), 7.0) * 3.0)+texture_color;
      gl_FragColor = color;

    //gl_FragColor = texture2D(qt_Texture0, qt_TexCoord0.st);
}
