#ifndef SNAKE3_SELECTIONRINGNODE3D_H
#define SNAKE3_SELECTIONRINGNODE3D_H

#include <functional>
#include <memory>

#include <snake3d/Renderer/Opengl/Model/Standard/MeshNode3D.h>

namespace Manager { class ShaderProgram; }
namespace Material { class ShaderMaterial; }
namespace Model { class ArrayMesh; }

namespace Model {
    // Generic, self-contained selection ring: a flat translucent annulus laid on
    // the XZ ground plane under a unit/object, with a slow pulse. Decoupled from any
    // game or the debug ManipulatorHandler (unlike snake3's MarkRingNode3D) - it owns
    // its mesh + material and loads its shader from the engine shader prelude
    // (EngineShaders/snake3d/selection_ring.*), shared across instances via the
    // ResourceManager. Starts hidden; the game toggles setVisible() from its selection
    // state. Does not cast shadows.
    //
    // It is a translucent decal (depthWrite off): render it AFTER the opaque ground so
    // its soft edges blend over the terrain (rendering it before would alpha-blend the
    // edges over the clear color, leaving a dark rim).
    class SelectionRingNode3D : public MeshNode3D {
    public:
        SelectionRingNode3D(
            const std::shared_ptr<Tools::ContextState> &contextState,
            const std::shared_ptr<Manager::ResourceManager> &resourceManager,
            float radius = 1.9f,
            const glm::vec3 &color = glm::vec3(0.20f, 1.0f, 0.35f));

        void setColor(const glm::vec3 &color) const;

        // Place the ring under a world point. Handles the Scale*Translate convention
        // (the node's scale = radius, so setPosition lands in pre-scale space). `lift`
        // raises it slightly above the ground to avoid z-fighting. In conforming mode
        // (see setHeightQuery) this also rebuilds the annulus when the point moved.
        void setGroundPosition(const glm::vec3 &worldPos, float lift = 0.0f);

        // OPTIONAL terrain-conforming mode: when a height query is set, the ring stops
        // being a flat decal - its annulus mesh is rebuilt (lazily, on movement) so
        // every vertex follows the ground surface under it. On curved ground (a bank
        // lip, a hill crest) no flat plane fits: tilted to the slope it floats off the
        // low side, kept flat it pokes out of the slope face. The query must return
        // the WORLD height of the ground at (x, z) - ideally of the RENDERED terrain
        // mesh (interpolated over its triangles), not of an analytic height field:
        // between grid vertices the rendered chords deviate from the smooth field and
        // the difference is enough to bury parts of the ring. Pass nullptr to revert
        // to the flat decal.
        void setHeightQuery(std::function<float(float, float)> query);

        [[nodiscard]] float getRadius() const { return radius; }

        // Decals don't cast shadows.
        void renderShadows(const std::shared_ptr<Manager::Camera> &camera, const glm::mat4 &projection,
                           float dt, const glm::mat4 &parentTransform) const override {}

    private:
        static std::shared_ptr<Manager::ShaderProgram> ringShader(
            const std::shared_ptr<Manager::ResourceManager> &resourceManager);

        // (Re)build the conforming annulus for the given ground point.
        void buildConformingMesh(const glm::vec3 &worldPos);

        std::shared_ptr<Material::ShaderMaterial> material;
        float radius;
        std::function<float(float, float)> heightQuery;   // empty -> flat decal
        std::shared_ptr<ArrayMesh> conformingMesh;        // the rebuilt annulus
        std::shared_ptr<StandardMesh> flatMesh;           // the original flat plane
        glm::vec3 conformBuiltFor{1e9f};                  // worldPos of the last build
    };
} // Model

#endif // SNAKE3_SELECTIONRINGNODE3D_H
