#include "ImGuiOverlay.h"

#include <iostream>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "CollisionShapeHandler.h"
#include "ManipulatorHandler.h"
#include "LightsHandler.h"
#include "PositionHandler.h"
#include "RotationHandler.h"
#include "ScaleHandler.h"
#include "../../Lights/Light.h"
#include "../../Renderer/Opengl/Model/Collision/CollisionShape3D.h"
#include "../../Renderer/Opengl/Scene/Scene.h"
#include "../../Lights/DirectionalLight.h"
#include "../../Lights/OrientableLight.h"
#include "../../Lights/PointLight.h"
#include "../../Lights/SpotLight.h"
#include "../../Manager/Camera.h"
#include "../../Manager/RenderManager.h"
#include "../../Renderer/Opengl/RenderStats.h"
#include "../../Tools/Transform.h"

#include <glm/geometric.hpp>
#include <iomanip>
#include <set>
#include <sstream>

namespace Handler::Debug {
    ImGuiOverlay::ImGuiOverlay(GLFWwindow* window,
                               std::shared_ptr<Manager::RenderManager> renderManager)
        : renderManager(std::move(renderManager)) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = nullptr;
        ImGui::StyleColorsDark();

        // GLSL 130 = OpenGL 3.0+. Snake3 main.cpp create window v 3.3 Compat,
        // takže 130 sedí (vyšší GLSL by způsobil core profile mismatch).
        if (!ImGui_ImplGlfw_InitForOpenGL(window, true)) {
            std::cerr << "[ImGuiOverlay] ImGui_ImplGlfw_InitForOpenGL failed\n";
            return;
        }
        if (!ImGui_ImplOpenGL3_Init("#version 130")) {
            std::cerr << "[ImGuiOverlay] ImGui_ImplOpenGL3_Init failed\n";
            return;
        }
        initialized = true;
    }

    ImGuiOverlay::~ImGuiOverlay() {
        if (!initialized) return;
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void ImGuiOverlay::setManipulatorHandler(std::shared_ptr<ManipulatorHandler> handler) {
        manipulatorHandler = std::move(handler);
    }

    void ImGuiOverlay::setScene(std::weak_ptr<Scenes::Scene> sc) {
        scene = std::move(sc);
    }

    void ImGuiOverlay::beginFrame() {
        if (!initialized) return;
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void ImGuiOverlay::renderPanels() {
        if (!initialized) return;
        drawEnginePanel();
        drawObjectInspector();
    }

    void ImGuiOverlay::endFrame() {
        if (!initialized) return;
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    bool ImGuiOverlay::wantsMouseInput() const {
        if (!initialized) return false;
        return ImGui::GetIO().WantCaptureMouse;
    }

    bool ImGuiOverlay::wantsKeyboardInput() const {
        if (!initialized) return false;
        return ImGui::GetIO().WantCaptureKeyboard;
    }

    void ImGuiOverlay::drawEnginePanel() const {
        if (!renderManager) return;

        // Default position v levém horním rohu, lze přetáhnout.
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin("Engine", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::End();
            return;
        }

        ImGui::Text("FPS: %.1f (%.2f ms/frame)",
                    ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
        ImGui::Text("Draw calls: %d (programs: %d)",
                    Renderer::RenderStats::drawCallsLastFrame,
                    Renderer::RenderStats::uniqueProgramsLastFrame);
        const auto& perPass = Renderer::RenderStats::drawsPerPassLastFrame;
        ImGui::Text("  Main: %d  Shadow: %d  Reflection: %d",
                    perPass[static_cast<int>(Renderer::RenderPass::Main)],
                    perPass[static_cast<int>(Renderer::RenderPass::Shadow)],
                    perPass[static_cast<int>(Renderer::RenderPass::Reflection)]);
        const float cpuMs = Renderer::RenderStats::mainPassMsCpuOnly;
        const float totalMs = Renderer::RenderStats::mainPassMsCpu;
        const float gpuMs = totalMs - cpuMs;
        ImGui::Text("Main pass: %.1f ms (CPU %.1f + GPU %.1f)", totalMs, cpuMs, gpuMs);
        ImGui::Text("Frame phases: upd %.1f  phys %.1f  render %.1f  swap %.1f",
                    Renderer::RenderStats::updateMs,
                    Renderer::RenderStats::physicsMs,
                    Renderer::RenderStats::renderMs,
                    Renderer::RenderStats::swapMs);
        ImGui::Text("  phys: aabb %.1f  pairs %.1f (%d tested, %d static + %d dyn)",
                    Renderer::RenderStats::physAabbMs,
                    Renderer::RenderStats::physPairsMs,
                    Renderer::RenderStats::pairsTested,
                    Renderer::RenderStats::collidersStatic,
                    Renderer::RenderStats::collidersDynamic);
        ImGui::Text("  upd: net %.1f  scene %.1f  hud %.1f",
                    Renderer::RenderStats::updateNetMs,
                    Renderer::RenderStats::updateSceneMs,
                    Renderer::RenderStats::updateHudMs);
        ImGui::Separator();

        // Rendering toggles - duplikují keyboard shortcuts (V/F/B/F2), ale
        // poskytují discoverable UI + state preview bez nutnosti pamatovat
        // klávesy.
        ImGui::TextDisabled("Rendering toggles");
        bool shadows = false;  // RenderManager dnes nemá isShadowsEnabled getter.
        bool fog = renderManager->isFogEnabled();
        bool reflections = renderManager->isReflectionsEnabled();
        // Bloom/shadows toggle - jediný způsob jak zjistit jejich stav je
        // přes toggle (RenderManager nemá getter). Read-only checkbox by lhal,
        // takže ukážeme klávesovou nápovědu:
        ImGui::TextDisabled("  V = shadows, B = bloom (no getter)");
        if (ImGui::Checkbox("Fog (F)", &fog)) {
            renderManager->toggleFog();
        }
        if (ImGui::Checkbox("Reflections (F2)", &reflections)) {
            renderManager->toggleReflections();
        }

        // Collision shapes - globální toggle všech CollisionShape3D ve scene
        // graphu. Walking scene tree (level má 2k+ floor cells jako mesh nodes)
        // je per-frame moc drahé pro 60 FPS - refresh cache každých ~30 frames,
        // explicit toggle stejně přepíše state hned.
        if (const auto scn = scene.lock()) {
            constexpr int refreshInterval = 30;
            if (collisionCountFrameCounter++ >= refreshInterval) {
                collisionCountFrameCounter = 0;
                cachedCollisionTotal = 0;
                cachedCollisionVisible = 0;
                scn->collectCollisionShapeCounts(cachedCollisionTotal, cachedCollisionVisible);
            }
            bool allVisible = cachedCollisionTotal > 0 && cachedCollisionTotal == cachedCollisionVisible;
            if (ImGui::Checkbox("Collision shapes", &allVisible)) {
                scn->setCollisionShapesVisible(allVisible);
                // Force-refresh cache na další frame, ať checkbox state odpovídá
                // realitě hned (bez 0.5s zpoždění než natiká counter).
                collisionCountFrameCounter = refreshInterval;
            }
        }

        ImGui::Separator();
        ImGui::TextDisabled("Shader cache");
        // ShaderRegistry inspector - počet cached programů + hot reload trigger.
        if (renderManager) {
            // RenderManager nedrží registry přímo - jdeme přes ResourceManager.
            // Pro MVP necháváme inspector minimal; full per-program list je
            // v F1 iter 3 plánu.
            if (ImGui::Button("Reload shaders (F10)")) {
                renderManager->reloadShaders();
                std::cout << "[ImGuiOverlay] reloadShaders requested\n";
            }
        }

        ImGui::End();
    }

    namespace {
        enum class InspectorKind { Light, Mesh, CollisionShape };

        struct InspectorItem {
            std::string label;
            std::shared_ptr<Node3D::Transform> transform;
            InspectorKind kind;
        };

        std::string buildLightLabel(const std::shared_ptr<Lights::Light>& light,
                                    int& dCount, int& sCount, int& pCount) {
            std::ostringstream s;
            if (std::dynamic_pointer_cast<Lights::DirectionalLight>(light)) {
                s << "Light: Directional " << dCount++;
            } else if (std::dynamic_pointer_cast<Lights::SpotLight>(light)) {
                s << "Light: Spot " << sCount++;
            } else if (std::dynamic_pointer_cast<Lights::PointLight>(light)) {
                s << "Light: Point " << pCount++;
            } else if (std::dynamic_pointer_cast<Lights::OrientableLight>(light)) {
                s << "Light: Orientable (generic)";
            } else {
                s << "Light";
            }
            return s.str();
        }

        std::string meshLabel(const std::shared_ptr<Model::MeshNode3D>& m, const InspectorKind k) {
            const std::string prefix = (k == InspectorKind::CollisionShape) ? "Shape: " : "Mesh: ";
            std::string name = m->getName();
            if (!name.empty()) {
                return prefix + name;
            }
            // Fallback pro unnamed: typ + pozice. Pomáhá rozlišit více
            // unnamed objektů v dropdownu (např. 4 torchNodes nepojmenovaných
            // v TorchScene). Pojmenovat explicit přes setName() je lepší,
            // ale tady aspoň label je unique a informativní.
            const auto pos = m->getPosition();
            std::ostringstream s;
            s << prefix << "(@" << std::fixed << std::setprecision(1)
              << pos.x << "," << pos.y << "," << pos.z << ")";
            return s.str();
        }

        // Sjednotí items ze všech debug handlerů do jedné dropdown listy.
        // Light a MeshNode3D mají disjoint typy. MeshNode3D může být v
        // position/scale/rotation/collision handleru zároveň - dedup přes
        // pointer identity, kind = první kde se objevil (collision má prioritu
        // pro typ-specific fieldset).
        std::vector<InspectorItem> collectItems(const ManipulatorHandler& mh) {
            std::vector<InspectorItem> out;
            if (const auto lh = mh.getLightsHandler()) {
                int dCount = 0, sCount = 0, pCount = 0;
                for (const auto& l : lh->getItems()) {
                    out.push_back({buildLightLabel(l, dCount, sCount, pCount), l, InspectorKind::Light});
                }
            }

            std::set<Model::MeshNode3D*> seen;
            // Collision shape first - prioritní typ pro fieldset dispatch.
            if (const auto csh = mh.getCollisionShapeHandler()) {
                for (const auto& m : csh->getItems()) {
                    if (!m || seen.contains(m.get())) continue;
                    seen.insert(m.get());
                    out.push_back({meshLabel(m, InspectorKind::CollisionShape), m, InspectorKind::CollisionShape});
                }
            }
            auto addMesh = [&](const auto& vec) {
                for (const auto& m : vec) {
                    if (!m || seen.contains(m.get())) continue;
                    seen.insert(m.get());
                    out.push_back({meshLabel(m, InspectorKind::Mesh), m, InspectorKind::Mesh});
                }
            };
            if (const auto h = mh.getPositionHandler()) addMesh(h->getItems());
            if (const auto h = mh.getScaleHandler())    addMesh(h->getItems());
            if (const auto h = mh.getRotationHandler()) addMesh(h->getItems());
            return out;
        }

        // Propaguje selection do všech handlerů (aby keyboard mody F5-F9
        // viděly stejný aktivní objekt jako GUI). Žádný camera side-effect -
        // ten řeší volající přes Camera::focusOn separately.
        void propagateSelection(const ManipulatorHandler& mh, const InspectorItem& item) {
            if (item.kind == InspectorKind::Light) {
                if (const auto lh = mh.getLightsHandler()) {
                    lh->setActiveItem(std::dynamic_pointer_cast<Lights::Light>(item.transform));
                }
                return;
            }
            const auto mesh = std::dynamic_pointer_cast<Model::MeshNode3D>(item.transform);
            if (!mesh) return;
            auto inItems = [&](const auto& vec) {
                return std::find(vec.begin(), vec.end(), mesh) != vec.end();
            };
            if (const auto h = mh.getPositionHandler(); h && inItems(h->getItems())) {
                h->setActiveItem(mesh);
            }
            if (const auto h = mh.getScaleHandler(); h && inItems(h->getItems())) {
                h->setActiveItem(mesh);
            }
            if (const auto h = mh.getRotationHandler(); h && inItems(h->getItems())) {
                h->setActiveItem(mesh);
            }
            if (item.kind == InspectorKind::CollisionShape) {
                if (const auto h = mh.getCollisionShapeHandler()) {
                    h->setActiveItem(std::dynamic_pointer_cast<CollisionShape::CollisionShape3D>(mesh));
                }
            }
        }

        void drawTransformFieldset(Node3D::Transform& t) {
            ImGui::TextDisabled("Transform");
            glm::vec3 pos = t.getPosition();
            if (ImGui::DragFloat3("Position", &pos.x, 0.1f)) {
                t.setPosition(pos);
            }
            float rotX = t.getRotationX();
            float rotY = t.getRotationY();
            float rotZ = t.getRotationZ();
            bool rotChanged = false;
            rotChanged |= ImGui::DragFloat("Rotation X", &rotX, 0.5f, -360.0f, 360.0f, "%.1f deg");
            rotChanged |= ImGui::DragFloat("Rotation Y", &rotY, 0.5f, -360.0f, 360.0f, "%.1f deg");
            rotChanged |= ImGui::DragFloat("Rotation Z", &rotZ, 0.5f, -360.0f, 360.0f, "%.1f deg");
            if (rotChanged) {
                t.setRotationX(rotX);
                t.setRotationY(rotY);
                t.setRotationZ(rotZ);
            }
            glm::vec3 scl = t.getScale();
            if (ImGui::DragFloat3("Scale", &scl.x, 0.01f, 0.0f, 100.0f)) {
                t.setScale(scl);
            }
        }

        void drawLightFieldset(Lights::Light& light) {
            ImGui::TextDisabled("Light color components");
            glm::vec3 ambient = light.getAmbient();
            if (ImGui::ColorEdit3("Ambient", &ambient.x, ImGuiColorEditFlags_Float)) {
                light.setAmbient(ambient);
            }
            glm::vec3 diffuse = light.getDiffuse();
            if (ImGui::ColorEdit3("Diffuse", &diffuse.x, ImGuiColorEditFlags_Float)) {
                light.setDiffuse(diffuse);
            }
            glm::vec3 specular = light.getSpecular();
            if (ImGui::ColorEdit3("Specular", &specular.x, ImGuiColorEditFlags_Float)) {
                light.setSpecular(specular);
            }
            if (auto* orientable = dynamic_cast<Lights::OrientableLight*>(&light)) {
                ImGui::Separator();
                ImGui::TextDisabled("Direction (auto-normalized)");
                glm::vec3 dir = orientable->getDirection();
                if (ImGui::DragFloat3("Direction", &dir.x, 0.05f, -3.0f, 3.0f)) {
                    if (glm::length(dir) > 1e-4f) {
                        orientable->setDirection(glm::normalize(dir));
                    }
                }
                const auto live = orientable->getDirection();
                ImGui::TextDisabled("  unit: %.2f, %.2f, %.2f", live.x, live.y, live.z);
            }
        }

        void drawCollisionFieldset(CollisionShape::CollisionShape3D& shape) {
            ImGui::TextDisabled("Collision");
            uint32_t layer = shape.getCollisionLayer();
            uint32_t mask = shape.getCollisionMask();
            ImGui::Text("Layer: 0x%08X", layer);
            ImGui::Text("Mask:  0x%08X", mask);
            ImGui::Text("Colliding bodies: %zu", shape.getCollidingBodies().size());
        }

        // Vypíše Transform + (Light | Collision) atributy aktivního objektu
        // do stdout v podobě validních C++ volání. Umožňuje copy-paste hodnot
        // do scene init kódu po laďění přes ImGui.
        void printObjectAttributes(Node3D::Transform& t) {
            const auto pos = t.getPosition();
            const auto scl = t.getScale();
            std::cout << "// === Inspector dump ===\n";
            std::cout << std::fixed << std::setprecision(4);
            std::cout << "setPosition({" << pos.x << "f, " << pos.y << "f, " << pos.z << "f});\n";
            std::cout << "setRotationX(" << t.getRotationX() << "f);\n";
            std::cout << "setRotationY(" << t.getRotationY() << "f);\n";
            std::cout << "setRotationZ(" << t.getRotationZ() << "f);\n";
            std::cout << "setScale({" << scl.x << "f, " << scl.y << "f, " << scl.z << "f});\n";

            if (auto* light = dynamic_cast<Lights::Light*>(&t)) {
                const auto a = light->getAmbient();
                const auto d = light->getDiffuse();
                const auto s = light->getSpecular();
                std::cout << "setAmbient({" << a.x << "f, " << a.y << "f, " << a.z << "f});\n";
                std::cout << "setDiffuse({" << d.x << "f, " << d.y << "f, " << d.z << "f});\n";
                std::cout << "setSpecular({" << s.x << "f, " << s.y << "f, " << s.z << "f});\n";
                if (auto* orientable = dynamic_cast<Lights::OrientableLight*>(light)) {
                    const auto dir = orientable->getDirection();
                    std::cout << "setDirection({" << dir.x << "f, " << dir.y << "f, " << dir.z << "f});\n";
                }
            } else if (auto* shape = dynamic_cast<CollisionShape::CollisionShape3D*>(&t)) {
                std::cout << "setCollisionLayer(0x" << std::hex << shape->getCollisionLayer() << ");\n";
                std::cout << "setCollisionMask(0x"  << shape->getCollisionMask()  << ");\n";
                std::cout << std::dec;
            }
            std::cout << "// === end ===" << std::endl;
        }
    }

    void ImGuiOverlay::drawObjectInspector() const {
        if (!manipulatorHandler) return;

        ImGui::SetNextWindowPos(ImVec2(10, 200), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(520, 0), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin("Object Inspector", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::End();
            return;
        }

        const auto items = collectItems(*manipulatorHandler);
        if (items.empty()) {
            ImGui::TextDisabled("No objects registered for debug.");
            ImGui::End();
            return;
        }

        // Inspector vlastní selection - drop priority resolution z handlerů,
        // GUI zachovává to co user kliknul. Lock weak_ptr a porovnávej proti
        // items pro combo preview.
        auto active = inspectorSelected.lock();

        // Pokud nic není vybráno, default na 1. item (a propaguj do handlerů).
        if (!active && !items.empty()) {
            inspectorSelected = items[0].transform;
            active = items[0].transform;
            propagateSelection(*manipulatorHandler, items[0]);
        }

        const char* preview = "(none)";
        for (const auto& it : items) {
            if (it.transform == active) {
                preview = it.label.c_str();
                break;
            }
        }

        if (ImGui::BeginCombo("Active object", preview)) {
            for (size_t i = 0; i < items.size(); ++i) {
                const auto& it = items[i];
                const bool isSelected = (it.transform == active);
                // PushID + label je nutné: ImGui Selectable využívá label jako
                // ID, takže více "Mesh: (unnamed)" by kolidovalo. PushID idx
                // dá každému selectable unikátní ID bez ohledu na label.
                ImGui::PushID(static_cast<int>(i));
                const bool clicked = ImGui::Selectable(it.label.c_str(), isSelected);
                ImGui::PopID();
                if (clicked) {
                    inspectorSelected = it.transform;
                    propagateSelection(*manipulatorHandler, it);
                    // Camera one-shot teleport + free movement (žádný sticky).
                    // Šipky / Ctrl+myš / WASD fungují normálně po focusOn.
                    if (renderManager) {
                        if (const auto cam = renderManager->getCamera()) {
                            cam->focusOn(it.transform);
                        }
                    }
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        if (!active) {
            ImGui::End();
            return;
        }

        // Tlačítko "Focus camera" - reset kamery na vybraný objekt. Po
        // delším bloudění s WASD/Ctrl-myší se uživatel může chtít vrátit
        // k objektu, který má v Inspectoru, bez znovuvolení v dropdownu.
        if (ImGui::Button("Focus camera")) {
            if (renderManager) {
                if (const auto cam = renderManager->getCamera()) {
                    cam->focusOn(active);
                }
            }
        }
        ImGui::SetItemTooltip("Reset camera position to the selected object (one-shot teleport, no follow).");

        ImGui::SameLine();
        if (ImGui::Button("Print")) {
            printObjectAttributes(*active);
        }
        ImGui::SetItemTooltip(
            "Print transform + type-specific attributes to stdout as C++ "
            "set*() calls. Copy-paste back into scene init code.");

        // Wider label column - default 100px je málo pro "Rotation X" + value.
        ImGui::PushItemWidth(-160.0f);

        ImGui::Separator();
        drawTransformFieldset(*active);

        if (auto* light = dynamic_cast<Lights::Light*>(active.get())) {
            ImGui::Separator();
            drawLightFieldset(*light);
        } else if (auto* shape = dynamic_cast<CollisionShape::CollisionShape3D*>(active.get())) {
            ImGui::Separator();
            drawCollisionFieldset(*shape);
        }

        ImGui::PopItemWidth();
        ImGui::End();
    }
} // Handler::Debug
