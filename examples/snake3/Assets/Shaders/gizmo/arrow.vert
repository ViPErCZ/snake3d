#version 330 core
layout (location = 0) in vec3 aPos;

// Matice
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Klasická transformace: Model -> View -> Projection
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}