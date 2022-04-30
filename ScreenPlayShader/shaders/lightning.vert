#version 440

layout(location=0) in vec2 qt_TexCoord0;
layout(location = 0) out vec4 fragColor;
layout(binding = 1) uniform sampler2D source;

layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
    float qt_Opacity;
    float time;
    vec2 resolution;
} ubuf;


float hash( float n ) { return fract(sin(n)*753.5453123); }

// Slight modification of iq's noise function.
float noise( in vec2 x )
{
    vec2 p = floor(x);
    vec2 f = fract(x);
    f = f*f*(3.0-2.0*f);

    float n = p.x + p.y*157.0;
    return mix(
                    mix( hash(n+  0.0), hash(n+  1.0),f.x),
                    mix( hash(n+157.0), hash(n+158.0),f.x),
            f.y);
}


float fbm(vec2 p, vec3 a)
{
     float v = 0.0;
     v += noise(p*a.x)*1.20;
     v += noise(p*a.y)*1.20;
     v += noise(p*a.z)*.125;
     return v;
}

vec3 drawLines( vec2 uv, vec3 fbmOffset, vec3 color1, vec3 color2, float time )
{
    float timeVal = time * 0.1;
    vec3 finalColor = vec3( 0.0 );
    for( int i=0; i < 3; ++i )
    {
        float indexAsFloat = float(i);
        float amp = 40.0 + (indexAsFloat*10.0);
        float period = 2.0 + (indexAsFloat+2.0);
        float thickness = mix( 0.9, 1.0, noise(uv*10.0) );
        float t = abs( 0.9 / (sin(uv.x + fbm( uv + timeVal * period, fbmOffset )) * amp) * thickness );

        finalColor +=  t * color1;
    }

    for( int i=0; i < 0; ++i )
    {
        float indexAsFloat = float(i);
        float amp = 40.0 + (indexAsFloat*7.0);
        float period = 2.0 + (indexAsFloat+8.0);
        float thickness = mix( 0.7, 1.0, noise(uv*10.0) );
        float t = abs( 0.8 / (sin(uv.x + fbm( uv + timeVal * period, fbmOffset )) * amp) * thickness );

        finalColor +=  t * color2 * 0.6;
    }

    return finalColor;
}

void main()
{

    //vec2 uv = ( coord.xy / resolution.xy ) * 1.0 - 1.8;
    //uv.x *= resolution.x/resolution.y;
    //uv.xy = uv.yx;


    vec3 lineColor1 = vec3( 2.3, 0.5, .5 );
    vec3 lineColor2 = vec3( 0.3, 0.5, 2.5 );

    vec3 finalColor = vec3(0.0);


    float t = sin( ubuf.time ) * 0.5 + 0.5;
    float pulse = mix( 0.10, 0.20, t);

    finalColor += drawLines( qt_TexCoord0, vec3( 1.0, 1.0, 8.0), lineColor2, lineColor2,  ubuf.time );

    fragColor = vec4( finalColor, 1.0 );

}

