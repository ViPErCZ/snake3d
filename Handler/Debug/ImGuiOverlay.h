#ifndef SNAKE3_IMGUIOVERLAY_H
#define SNAKE3_IMGUIOVERLAY_H

#include <memory>

struct GLFWwindow;

namespace Manager {
    class RenderManager;
}

namespace Scenes {
    class Scene;
}

namespace Node3D {
    class Transform;
}

namespace Physic {
    class CollisionSystem3D;
    namespace Dynamics {
        class DynamicBody;
    }
}

namespace Handler::Debug {
    class ManipulatorHandler;
}

namespace Handler::Debug {
    // Debug-only ImGui overlay. Engine layer (generic - mode-driven panels read
    // from ManipulatorHandler; engine toggles read from RenderManager). Game
    // layer can be added later via injection of additional panel providers.
    //
    // Lifetime: created after GLFW window + OpenGL context exist, destroyed
    // before window. Application owns one instance in debug builds only.
    class ImGuiOverlay {
    public:
        ImGuiOverlay(GLFWwindow* window,
                     std::shared_ptr<Manager::RenderManager> renderManager);
        ~ImGuiOverlay();

        ImGuiOverlay(const ImGuiOverlay&) = delete;
        ImGuiOverlay& operator=(const ImGuiOverlay&) = delete;

        // Late binding: ManipulatorHandler is created after the scene's first
        // prepareScene() steps, so we wire it up once available.
        void setManipulatorHandler(std::shared_ptr<ManipulatorHandler> handler);

        // Active scene reference for engine-level scene traversal (e.g. bulk
        // toggle všech collision shapes přes scene graf). Weak reference -
        // app vlastní scene lifetime, overlay si jen půjčuje getter.
        void setScene(std::weak_ptr<Scenes::Scene> scene);

        // Inspector pauses the DynamicBody owning the currently-selected
        // CollisionShape3D so editing scale/transform doesn't fight the physics
        // step (resolveTopContact would otherwise re-snap the head onto the
        // floor every tick while user drags shape fields). Weak ref - app owns
        // collision system lifetime.
        void setCollisionSystem(std::weak_ptr<Physic::CollisionSystem3D> cs);

        // Begin a new ImGui frame. Call once per game frame, before drawing
        // any windows (i.e. before any ImGui::Begin in the engine/game).
        void beginFrame();

        // Render built-in engine panels (mode-aware Lights/Position/etc. +
        // Engine toggles). Call after beginFrame() and after scene render so
        // panels stack on top of the scene.
        void renderPanels();

        // Flush ImGui draw commands to the current framebuffer. Call as the
        // last GPU op before glfwSwapBuffers.
        void endFrame();

        // Did ImGui capture the input this frame? Game input handlers should
        // skip propagation when this returns true (ImGui owns the mouse/kb).
        [[nodiscard]] bool wantsMouseInput() const;
        [[nodiscard]] bool wantsKeyboardInput() const;

    private:
        void drawEnginePanel() const;
        void drawObjectInspector() const;
        void drawShaderInspector() const;

        std::shared_ptr<Manager::RenderManager> renderManager;
        std::shared_ptr<ManipulatorHandler> manipulatorHandler;
        std::weak_ptr<Scenes::Scene> scene;
        std::weak_ptr<Physic::CollisionSystem3D> collisionSystem;

        // Currently-frozen DynamicBody (because inspector selected a CollisionShape
        // whose owner has one). Cleared when selection changes. Weak_ptr so we
        // don't extend its lifetime. Uses DynamicBody::setDebugFrozen, which is
        // separate from `enabled` so SnakeMoveHandler-style gameplay toggling
        // can't undo the UI pause.
        mutable std::weak_ptr<Physic::Dynamics::DynamicBody> inspectorPausedBody;

        // Inspector vlastní "selected" state - nezávislý na handler::activeItem.
        // Více handlerů má vlastní activeItem (Light, Position, Scale, Rotation,
        // Collision) a jejich resolution priority by zaměnila to co user kliknul.
        // mutable protože drawObjectInspector je const a updatuje selection
        // přes click v Selectable.
        mutable std::weak_ptr<Node3D::Transform> inspectorSelected;

        // Cache pro tristate checkbox "Collision shapes". Scene::collect-
        // CollisionShapeCounts walkuje celý scene graf (level boxy mají 2k+
        // floor cells), per-frame call srazí FPS pod 10. Refresh každých
        // ~30 frames (cca 0.5s) nebo na explicit toggle action stačí.
        mutable int cachedCollisionTotal = 0;
        mutable int cachedCollisionVisible = 0;
        mutable int collisionCountFrameCounter = 0;

        bool initialized = false;
    };
} // Handler::Debug

#endif //SNAKE3_IMGUIOVERLAY_H
