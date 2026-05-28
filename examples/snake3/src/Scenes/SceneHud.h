#ifndef SNAKE3_SCENEHUD_H
#define SNAKE3_SCENEHUD_H

#include <memory>
#include <string>

#include "Manager/ResourceManager.h"
#include "Renderer/Opengl/Material/ShaderMaterial.h"
#include "Renderer/Opengl/Material/Uniform/FadeInUniform.h"
#include "Renderer/Opengl/Material/Uniform/FadeOutUniform.h"
#include "../Renderer/Opengl/Model/Game/RadarMeshNode2D.h"
#include "Renderer/Opengl/Model/Standard/2D/LabelNode2D.h"
#include "Renderer/Opengl/Model/Standard/2D/MeshNode2D.h"
#include "Renderer/Opengl/Model/Standard/2D/QuadNode2D.h"
#include "Tools/ContextState.h"

namespace Scenes {
    class Scene;

    class SceneHud final {
    public:
        SceneHud(Scene &parent,
                 std::shared_ptr<Tools::ContextState> contextState,
                 std::shared_ptr<Manager::ResourceManager> resourceManager,
                 int width, int height);

        void initLabels();
        void initRadar();

        void resize(int width, int height);

        void setLevelText(int level, int eatCounter, int lives, int maxPoint);
        void setHelpVisible(bool visible) const;
        void setTilesCounterVisible(bool visible) const;
        [[nodiscard]] bool isHelpVisible() const;

        void triggerLabelFadeOut() const;
        void resetLabelFade() const;
        void triggerCounterFadeIn() const;

        void clearRadarItems() const;
        void addRadarItem(const std::shared_ptr<Model::MeshNode3D> &item, const glm::vec3 &color, const std::string &name) const;
        void setCoinVisible(bool visible);
        void setRadarVisible(bool visible) const;
        [[nodiscard]] bool isRadarVisible() const;
        void toggleRadar() const;
        void hideRadarItems() const;

        void tickRadarFade() const;

        void saveVisibility();
        void restoreVisibility();
        void markRadarVisibleForRestore();

    private:
        Scene &parent;
        std::shared_ptr<Tools::ContextState> contextState;
        std::shared_ptr<Manager::ResourceManager> resourceManager;
        int width;
        int height;

        std::shared_ptr<Model::LabelNode2D> tilesCounterText;
        std::shared_ptr<Model::MeshNode2D> helpText;
        std::shared_ptr<Model::MeshNode2D> tilesCounterNode;

        std::shared_ptr<Material::ShaderMaterial> radarExpansionIn;
        std::shared_ptr<Material::ShaderMaterial> radarExpansionOut;
        std::shared_ptr<Uniform::FadeInUniform> radarFadeInUniform;
        std::shared_ptr<Uniform::FadeOutUniform> radarFadeOutUniform;
        std::shared_ptr<Model::QuadNode2D> radarNode;
        std::shared_ptr<Model::RadarMeshNode2D> radarMeshNode;

        std::shared_ptr<Uniform::FadeOutUniform> labelFadeOutUniform;
        std::shared_ptr<Uniform::FadeInUniform> counterFadeInUniform;

        bool coinVisibleOnRadar = true;
        bool savedState = false;
        bool savedHelpVisible = false;
        bool savedTilesVisible = false;
        bool savedRadarVisible = false;
    };
}

#endif //SNAKE3_SCENEHUD_H
