#version 330 core

// Engine selection ring (snake3d/SelectionRingNode3D), laid flat on the ground
// (XZ plane) under an object. The mesh is a PlaneMesh authored in XZ with local
// coords in [-1, 1]; we pass them through so the fragment shader can draw an
// annulus. model/view/projection are set by ShaderMaterial::bind (custom-shader path).
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 vLocal;

void main() {
    vLocal = aPos.xz;                 // local plane coords, [-1,1] for PlaneMesh(2,2)
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
