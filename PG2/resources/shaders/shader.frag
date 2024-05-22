#version 460 core

in vec4 color; // input from vertex stage of graphics pipeline, automatically interpolated
in vec2 o_texture_coordinate;

out vec4 FragColor; // output color of current fragment: MUST be written


//uniforms
uniform sampler2D texture;


void main()
{
    //FragColor = vec4(color, 1.0f); // copy RGB color, add Alpha=1.0 (not transparent)
    //FragColor = color; //gives color without textures 
    FragColor = texture(texture, o_texture_coordinate);
}
