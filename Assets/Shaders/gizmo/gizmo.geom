#version 330 core

layout(lines) in;
layout(triangle_strip, max_vertices = 8) out;

uniform mat4 view;
uniform mat4 proj;
uniform float thickness;
uniform vec2 viewportSize;
uniform bool isArrowHead;
uniform int axisType; // 0 = X, 1 = Y, 2 = Z

in vec3 v_worldPos[];
out vec3 f_worldPos;

const float NDC_SCALE = 2.0;
const float NDC_OFFSET = 1.0;
const float HALF_FACTOR = 0.5;
const float ARROW_WIDTH_FACTOR = 2.5;// Šířka špičky šipky relativně k tloušťce čáry
const float ARROW_LENGTH_FACTOR = 3.0;// Délka špičky šipky relativně k tloušťce čáry

vec2 worldToScreenCoords(vec4 clipPos) {
    vec3 ndc = clipPos.xyz / clipPos.w;
    return vec2(
    (ndc.x * HALF_FACTOR + HALF_FACTOR) * viewportSize.x,
    (NDC_OFFSET - (ndc.y * HALF_FACTOR + HALF_FACTOR)) * viewportSize.y
    );
}

vec2 screenToNDC(vec2 screenPos) {
    float x = (screenPos.x / viewportSize.x) * NDC_SCALE - NDC_OFFSET;
    float y = NDC_OFFSET - (screenPos.y / viewportSize.y) * NDC_SCALE;
    return vec2(x, y);
}

void emitLineVertex(vec2 ndcPos, float clipZ, float clipW, vec3 worldPos) {
    gl_Position = vec4(ndcPos, clipZ / clipW, 1.0) * clipW;
    f_worldPos = worldPos;
    EmitVertex();
}

void main() {
    vec4 clipStart = proj * view * vec4(v_worldPos[0], 1.0);
    vec4 clipEnd = proj * view * vec4(v_worldPos[1], 1.0);

    vec2 screenStart = worldToScreenCoords(clipStart);
    vec2 screenEnd = worldToScreenCoords(clipEnd);

    if (isArrowHead) {
        vec2 lineDir = normalize(screenEnd - screenStart);
        vec2 normalDir = vec2(-lineDir.y, lineDir.x);
        float arrowWidth = thickness * ARROW_WIDTH_FACTOR;
        float arrowLength = thickness * ARROW_LENGTH_FACTOR;

        // Zde je ta klíčová změna - šipka musí být na konci pro každou osu
        vec2 arrowTip = screenEnd;  // Vždy použijeme konec čáry pro špičku
        vec2 arrowBase = arrowTip - lineDir * arrowLength;
        vec2 arrowLeft = arrowBase + normalDir * arrowWidth;
        vec2 arrowRight = arrowBase - normalDir * arrowWidth;

        vec2 ndcTip = screenToNDC(arrowTip);
        vec2 ndcLeft = screenToNDC(arrowLeft);
        vec2 ndcRight = screenToNDC(arrowRight);

        emitLineVertex(ndcLeft, clipEnd.z, clipEnd.w, v_worldPos[1]);
        emitLineVertex(ndcTip, clipEnd.z, clipEnd.w, v_worldPos[1]);
        emitLineVertex(ndcRight, clipEnd.z, clipEnd.w, v_worldPos[1]);

        EndPrimitive();
    } else {
        // Původní kód pro čáry zůstává stejný
        vec2 lineDir = normalize(screenEnd - screenStart);
        vec2 normalDir = vec2(-lineDir.y, lineDir.x);
        float halfThickness = thickness * HALF_FACTOR;

        vec2 offsetTopStart = screenStart + normalDir * halfThickness;
        vec2 offsetBottomStart = screenStart - normalDir * halfThickness;
        vec2 offsetTopEnd = screenEnd + normalDir * halfThickness;
        vec2 offsetBottomEnd = screenEnd - normalDir * halfThickness;

        vec2 ndcTopStart = screenToNDC(offsetTopStart);
        vec2 ndcBottomStart = screenToNDC(offsetBottomStart);
        vec2 ndcTopEnd = screenToNDC(offsetTopEnd);
        vec2 ndcBottomEnd = screenToNDC(offsetBottomEnd);

        emitLineVertex(ndcTopStart, clipStart.z, clipStart.w, v_worldPos[0]);
        emitLineVertex(ndcBottomStart, clipStart.z, clipStart.w, v_worldPos[0]);
        emitLineVertex(ndcTopEnd, clipEnd.z, clipEnd.w, v_worldPos[1]);
        emitLineVertex(ndcBottomEnd, clipEnd.z, clipEnd.w, v_worldPos[1]);
        EndPrimitive();
    }
}