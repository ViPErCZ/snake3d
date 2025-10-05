#ifndef TORCHRENDERER_H
#define TORCHRENDERER_H

#include "BaseRenderer.h"
#include "../../ItemsDto/Cube.h"
#include "../../Manager/ResourceManager.h"
#include "../../Manager/ShaderManager.h"

using namespace ItemsDto;
using namespace Manager;

namespace Renderer {

class TorchRenderer : public BaseRenderer {

    enum class Axis { None, X, Y, Z, RotateX, RotateY, RotateZ };

    Axis pickTranslateAxis(const glm::vec3 &worldCenter,
                           const glm::vec3 &rayOrigin,
                           const glm::vec3 &rayDir,
                           float gizmoScale,
                           const glm::vec2 &cursor,
                           int viewportWidth,
                           int viewportHeight,
                           float pixelThreshold = 16.0f);
    Axis pickRotationAxis(const glm::vec3 &worldCenter,
                                         const glm::vec2 &cursor,
                                         int viewportWidth,
                                         int viewportHeight,
                                         float pixelThreshold = 16.0f);
    enum class InteractionMode { None, Translating, Rotating };

public:
    TorchRenderer(Cube *cube, Camera *camera, const glm::mat4 &projection, ResourceManager* resManager);
    ~TorchRenderer() override;
    void render(float dt) override;
    void renderShadowMap() override;
    void beforeRender() override;
    void afterRender() override;

    glm::vec3 screenToWorldRay(const glm::vec2& screenPos, const glm::mat4& view, const glm::mat4& proj,
                           int viewportWidth, int viewportHeight);
    // glm::vec3 projectRayOntoAxis(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
    //                          const glm::vec3& axisOrigin, const glm::vec3& axisDir);
    void onMouseDown(const glm::vec2& cursor, int width, int height);
    void onMouseMove(const glm::vec2& cursor, int width, int height);
    void onMouseUp();
protected:
    void renderScene(const ShaderManager* shader);
    void computeLocalAABB(const std::vector<Vertex>& verts, glm::vec3& outMin, glm::vec3& outMax);
    void computeWorldAABB(const glm::mat4& model, const glm::vec3& localMin, const glm::vec3& localMax,
                      glm::vec3& outWorldMin, glm::vec3& outWorldMax);
    float computeGizmoScale(const glm::vec3& worldCenter, const glm::vec3& cameraPos,
                            float viewportHeight,
                            float desiredPixelSize);
    void drawGizmoAxes(const glm::vec3& worldCenter,
                   const glm::vec3& cameraPos,
                   const glm::mat4& view,
                   const glm::mat4& proj,
                   float viewportHeight);
    void drawRotationGizmo(const glm::vec3& worldCenter,
                       const glm::vec3& cameraPos,
                       const glm::mat4& view,
                       const glm::mat4& proj,
                       float viewportHeight,
                       int circleVertexCount);
    std::vector<glm::vec3> makeCircleForAxis(const glm::vec3& axis, int segments);
    bool closestPointsBetweenLines(const glm::vec3& p1, const glm::vec3& d1,
                                const glm::vec3& p2, const glm::vec3& d2,
                                glm::vec3& outPoint1, glm::vec3& outPoint2);
    glm::vec3 worldToScreen(const glm::vec3 &worldPos,
                                           const glm::mat4 &view,
                                           const glm::mat4 &proj,
                                           int width,
                                           int height);
    float pointToSegmentDistance2D(const glm::vec2& p, const glm::vec2& a, const glm::vec2& b);
    void updateHover(const glm::vec2 &cursor, int width, int height);
    Cube* cube;
    Mesh* mesh;
    Camera* camera;
    glm::mat4 projection;
    ResourceManager* resourceManager;
    ShaderManager* baseShader;
    TextureManager* texture;
    TextureManager* texture2;
    unsigned int quadVAO, quadVBO, gizmoVAO, gizmoVBO;
    unsigned int circleVAOX, circleVBOX;
    unsigned int circleVAOY, circleVBOY;
    unsigned int circleVAOZ, circleVBOZ;
    glm::vec3 currentWorldCenter;
    Axis activeAxis = Axis::None;
    Axis hoveredAxis = Axis::None;
    InteractionMode mode = InteractionMode::None;
    float startAxisParam = 0.0f;
    float currentAxisParam = 0.0f;
    float rotationStartAngle = 0.0f;
    glm::vec4 originalRotation[3];
    glm::vec3 dragLineOrigin = {};

    glm::vec3 startObjectPos;
    glm::vec3 startGrabPoint; // projekce kurzorového raye na osu při mouse down
    glm::vec3 axisDir;        // směr aktuální osy (unit)
    float gizmoBaseScale = 1.0f; // Derived from object size (world AABB)
    glm::vec3 currentWorldMin;
    glm::vec3 currentWorldMax;
};

} // Renderer

#endif //TORCHRENDERER_H
