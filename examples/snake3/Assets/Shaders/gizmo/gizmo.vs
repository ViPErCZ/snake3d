#version 330 core

layout(location=0) in vec3 aPos;

uniform mat4 view;
uniform mat4 proj;
uniform vec3 gizmoCenter;
uniform float scale;

out vec3 v_worldPos;

void main() {
    vec3 scaled = aPos * scale;
    vec4 world = vec4(gizmoCenter + scaled, 1.0);
    v_worldPos = world.xyz;
    gl_Position = proj * view * world;
}