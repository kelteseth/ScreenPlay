precision lowp float;

uniform float time;
uniform vec2 resolution;

void main( void ) {


        vec2 position = ( gl_FragCoord.xy / resolution.xy );

        vec3 color1 = vec3(1.0, 0.0, 0.0);
        vec3 color2 = vec3(0.97, 0.24, 0.24);
        float mixValue = distance(position,vec2(0,1));

        vec3 color = mix( color1, color2,mixValue);

        gl_FragColor = vec4(color,mixValue);

}
