#version 120

uniform mat4 camera_projection;
uniform mat4 camera_modelview;
uniform mat4 normal_matrix;
uniform float time;

varying vec4 position_3d_original;
varying vec4 position_3d_modelview;

varying vec3 normal;
varying vec4 color;




void main (void)
{
    float value = -0.2*(1.0-cos(time/10.0));
    vec4 translation = value*vec4(gl_Normal,0);

    gl_Position = camera_projection*camera_modelview*(gl_Vertex+translation);

    position_3d_original = gl_Vertex;
    position_3d_modelview = camera_modelview*gl_Vertex;
    color = gl_Color;

    vec4 normal4d = normal_matrix*vec4(normalize(gl_Normal),0.0);
    normal = normal4d.xyz;

    gl_TexCoord[0]=gl_MultiTexCoord0;
}
