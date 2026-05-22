#include "LightsHandler.h"

#include <algorithm>

#include "../../Lights/SpotLight.h"
#include "../../Renderer/Opengl/Model/Debug/DirectionalLightNode3D.h"

namespace Handler::Debug {
    LightsHandler::LightsHandler(const shared_ptr<ContextState> &contextState,
        const shared_ptr<ResourceManager> &resourceManager,
        const shared_ptr<Camera> &camera) : camera(camera), enabled(false) {
        cameraOriginalStickyPoint = camera->getStickyPoint();
        const auto shader = resourceManager->getShader("textShader");
        const auto font = make_shared<Font>("Assets/Fonts/OCRAEXT.TTF", 14);
        labelSettings = make_shared<LabelSettings>(font);
        focusText = make_shared<LabelNode2D>("", shader, labelSettings);
        focusText->setText("Focus: Ambient");
        focusTextNode = make_shared<MeshNode2D>(contextState, focusText, resourceManager);
        focusTextNode->setVisible(false);
        colorText = make_shared<LabelNode2D>("Color: -", shader, labelSettings);
        colorText->setText("Color: -");
        colorTextNode = make_shared<MeshNode2D>(contextState, colorText, resourceManager);
        colorTextNode->setVisible(false);
        positionText = make_shared<LabelNode2D>("Position: -", shader, labelSettings);
        positionText->setText("Position: -");
        positionTextNode = make_shared<MeshNode2D>(contextState, positionText, resourceManager);
        positionTextNode->setVisible(false);
        directionText = make_shared<LabelNode2D>("Direction: -", shader, labelSettings);
        directionText->setText("Direction: -");
        directionTextNode = make_shared<MeshNode2D>(contextState, directionText, resourceManager);
        directionTextNode->setVisible(false);

        dirLightNode = make_shared<DirectionalLightNode3D>(contextState, resourceManager->getShader("arrowGizmo"), resourceManager);
        dirLightNode->setVisible(false);
    }

    void LightsHandler::onDefaultHandler() {
    }

    void LightsHandler::onEventHandler(const unsigned key, int scancode, int action, const int mods, float deltaTime) {
        if (activeItem == nullptr) {
            return;
        }

        const auto light = focus == Focus::Direction ? dynamic_pointer_cast<OrientableLight>(activeItem) : nullptr;
        const float sensitivity = mods & GLFW_MOD_SHIFT ? 0.1f : 0.01f;

        switch (key) {
            case GLFW_KEY_RIGHT:
                if (enabled) {
                    if (focus == Focus::Ambient || focus == Focus::Diffuse || focus == Focus::Specular) {
                        auto color = getColorByFocus();
                        if (mods & GLFW_MOD_CONTROL) {
                            color.y += sensitivity;
                        } else if (mods & GLFW_MOD_ALT) {
                            color.z += sensitivity;
                        } else {
                            color.x += sensitivity;
                        }
                        setColorByFocus(color);
                    } else if (focus == Focus::Direction && light != nullptr) {
                        auto direction = light->getDirection();
                        if (mods & GLFW_MOD_CONTROL) {
                            direction.z += sensitivity;
                        } else if (mods & GLFW_MOD_ALT) {
                            direction.y += sensitivity;
                        } else {
                            direction.x += sensitivity;
                        }
                        light->setDirection(direction);
                    } else if (focus == Focus::Position) {
                        auto pos = activeItem->getPosition();
                        if (mods & GLFW_MOD_CONTROL) {
                            pos.z += sensitivity;
                        } else if (mods & GLFW_MOD_ALT) {
                            pos.y += sensitivity;
                        } else {
                            pos.x += sensitivity;
                        }
                        activeItem->setPosition(pos);
                    }
                }
                break;
            case GLFW_KEY_LEFT:
                if (enabled) {
                    if (focus == Focus::Ambient || focus == Focus::Diffuse || focus == Focus::Specular) {
                        auto color = getColorByFocus();
                        if (mods & GLFW_MOD_CONTROL) {
                            color.y -= sensitivity;
                        } else if (mods & GLFW_MOD_ALT) {
                            color.z -= sensitivity;
                        } else {
                            color.x -= sensitivity;
                        }
                        setColorByFocus(color);
                    } else if (focus == Focus::Direction && light != nullptr) {
                        auto direction = light->getDirection();
                        if (mods & GLFW_MOD_CONTROL) {
                            direction.z -= sensitivity;
                        } else if (mods & GLFW_MOD_ALT) {
                            direction.y -= sensitivity;
                        } else {
                            direction.x -= sensitivity;
                        }
                        light->setDirection(direction);
                    } else if (focus == Focus::Position) {
                        auto pos = activeItem->getPosition();
                        if (mods & GLFW_MOD_CONTROL) {
                            pos.z -= sensitivity;
                        } else if (mods & GLFW_MOD_ALT) {
                            pos.y -= sensitivity;
                        } else {
                            pos.x -= sensitivity;
                        }
                        activeItem->setPosition(pos);
                    }
                }
                break;
            case GLFW_KEY_KP_ADD:
                setColorByFocus(getColorByFocus());
                if (focus == Focus::Ambient) {
                    focus = Focus::Diffuse;
                    focusText->setText("Focus: Diffuse");
                } else if (focus == Focus::Diffuse) {
                    focus = Focus::Specular;
                    focusText->setText("Focus: Specular");
                } else if (focus == Focus::Specular) {
                    focus = Focus::Direction;
                    focusText->setText("Focus: Direction");
                } else if (focus == Focus::Direction) {
                    focus = Focus::Position;
                    focusText->setText("Focus: Position");
                } else {
                    focus = Focus::Ambient;
                    focusText->setText("Focus: Ambient");
                }
                break;
            case GLFW_KEY_TAB:
                if (enabled) {
                    activeItem = findNextItem();
                }
                break;
            case GLFW_KEY_SPACE:
                if (enabled) {
                    // print to console
                }
            default:
                break;
        }

        const auto pos = activeItem->getPosition();
        positionText->setText("Position: " + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ", " + std::to_string(pos.z));
        const auto lightActual = dynamic_pointer_cast<OrientableLight>(activeItem);
        if (lightActual != nullptr) {
            const auto direction = lightActual->getDirection();
            directionText->setText("Direction: " + std::to_string(direction.x) + ", " + std::to_string(direction.y) + ", " + std::to_string(direction.z));
            dirLightNode->setOrientableLight(lightActual);
            dirLightNode->setVisible(true);
        } else {
            directionText->setText("Direction: -");
            dirLightNode->setVisible(false);
        }
    }

    void LightsHandler::addItem(const shared_ptr<Light> &item) {
        items.push_back(item);

        if (activeItem == nullptr) {
            activeItem = item;
        }
    }

    void LightsHandler::setActiveItem(const shared_ptr<Light>& item) {
        // Pure data swap - žádný camera side-effect. GUI dropdown vyřeší
        // kamera teleport přes Camera::focusOn samostatně; keyboard mode
        // entry (F9) si ho dělá taky vlastní cestou pokud chce sticky.
        if (!item) return;
        const auto found = std::find(items.begin(), items.end(), item);
        if (found == items.end()) {
            items.push_back(item);
        }
        activeItem = item;
    }

    void LightsHandler::active() {
        enabled = true;
        if (nullptr != activeItem) {
            activeItem = findNextItem();
        }
        focusTextNode->setVisible(true);
        colorTextNode->setVisible(true);
        positionTextNode->setVisible(true);
        directionTextNode->setVisible(true);
        setColorByFocus(getColorByFocus());
    }

    void LightsHandler::deactivate() {
        enabled = false;
        focusTextNode->setVisible(false);
        colorTextNode->setVisible(false);
        positionTextNode->setVisible(false);
        directionTextNode->setVisible(false);
    }

    shared_ptr<Light> LightsHandler::findNextItem() {
        if (items.empty()) {
            return nullptr;
        }

        for (auto it = items.begin(); it != items.end(); ++it) {
            if (activeItem == *it) {
                if (it + 1 == items.end()) {
                    return *items.begin();
                }

                return *(it + 1);
            }
        }

        return nullptr;
    }

    glm::vec3 LightsHandler::getColorByFocus() const {
        switch (focus) {
            case Focus::Ambient:
                return activeItem->getAmbient();
            case Focus::Diffuse:
                return activeItem->getDiffuse();
            case Focus::Specular:
                return activeItem->getSpecular();
            default:
                return glm::vec3(0);
        }
    }

    void LightsHandler::setColorByFocus(glm::vec3 color) const {
        color = glm::clamp(color, 0.0f, 1.0f);
        colorText->setText("Color: " + std::to_string(getColorByFocus().x) + ", " + std::to_string(getColorByFocus().y) + ", " + std::to_string(getColorByFocus().z));
        switch (focus) {
            case Focus::Ambient:
                activeItem->setAmbient(color);
                break;
            case Focus::Diffuse:
                activeItem->setDiffuse(color);
                break;
            case Focus::Specular:
                activeItem->setSpecular(color);
                break;
            default:
                break;
        }
    }
}
