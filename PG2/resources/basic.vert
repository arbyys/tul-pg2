#version 460 core

in vec3 aPos;   // position: MUST exist
//in vec3 aColor; // any additional attributes are optional, any data type, etc.

uniform mat4 uMx_projection = mat4(1.0);
uniform mat4 uMx_model      = mat4(1.0);
uniform mat4 uMx_view       = mat4(1.0);

uniform vec4 uRGBA;

out vec4 color; // optional output attribute

// VS -> FS
//out vec2 o_texture_coordinate; //todo tohle je potřeba nevim kde to vzit

void main()
{
    //o_fragment_position = vec3(uMx_model * vec4(aPos, 1.0f));//todo later change
    // Outputs the positions/coordinates of all vertices, MUST WRITE
    //gl_Position = vec4(aPos, 1.0f);
    gl_Position = uMx_projection * uMx_view * uMx_model * vec4(aPos, 1.0f);
    //color = aColor; // copy color to output
    color = uRGBA;
}
