#include "SceneLightFactory.h"

namespace Scenes {
    SceneLights SceneLightFactory::create() {
        constexpr auto spotAmbientColor = glm::vec3(0.5f, 0.5f, 0.5f);
        constexpr auto spotSpecularColor = glm::vec3(0.1f, 0.1f, 0.1f);

        const auto spotLight = std::make_shared<Lights::SpotLight>();
        spotLight->setPosition({-1.7521, -0.75, -1.5f});
        spotLight->setDirection({0.0, -1.0, 0.0f});
        spotLight->setAmbient(spotAmbientColor);
        spotLight->setDiffuse({0.0f, 0.0f, 0.0f});
        spotLight->setSpecular(spotSpecularColor);
        spotLight->setCutOff(12.5);
        spotLight->setOuterCutOff(17.5);
        spotLight->setPulse(true);

        const auto spotLight2 = std::make_shared<Lights::SpotLight>();
        spotLight2->setPosition({3.67, 2.81, -1.5f});
        spotLight2->setDirection({1.9, 3.0, 0.0f});
        spotLight2->setAmbient(spotAmbientColor);
        spotLight2->setDiffuse({0.0f, 0.0f, 0.0f});
        spotLight2->setSpecular(spotSpecularColor);
        spotLight2->setCutOff(12.5);
        spotLight2->setOuterCutOff(17.5);
        spotLight2->setPulse(true);

        const auto spotLight3 = std::make_shared<Lights::SpotLight>();
        spotLight3->setPosition({-1.7521, 2.81, -1.5f});
        spotLight3->setDirection({0.0, 3.0, 0.0f});
        spotLight3->setAmbient(spotAmbientColor);
        spotLight3->setDiffuse({0.0f, 0.0f, 0.0f});
        spotLight3->setSpecular(spotSpecularColor);
        spotLight3->setCutOff(12.5);
        spotLight3->setOuterCutOff(17.5);
        spotLight3->setPulse(true);

        const auto spotLight4 = std::make_shared<Lights::SpotLight>();
        spotLight4->setPosition({3.67, -0.75, -1.5f});
        spotLight4->setDirection({2.0f, -1.0, 0.0f});
        spotLight4->setAmbient(spotAmbientColor);
        spotLight4->setDiffuse({0.0f, 0.0f, 0.0f});
        spotLight4->setSpecular(spotSpecularColor);
        spotLight4->setCutOff(12.5);
        spotLight4->setOuterCutOff(17.5);
        spotLight4->setPulse(true);

        const auto spotLight5 = std::make_shared<Lights::SpotLight>();
        spotLight5->setPosition({1.93, 0.43, -1.5f});
        spotLight5->setDirection({1.93f, 0.43, 0.0f});
        spotLight5->setAmbient(glm::vec3(1.0f, 0.95f, 0.8f));
        spotLight5->setDiffuse({0.991f, 0.982f, 0.305f});
        spotLight5->setSpecular({0.4f, 0.4f, 0.4f});
        spotLight5->setConstant(0.9f);
        spotLight5->setCutOff(7.5);
        spotLight5->setOuterCutOff(13.5);
        spotLight5->setPulse(true);
        spotLight5->setVisible(false);

        const auto pointLight1 = std::make_shared<Lights::PointLight>();
        pointLight1->setPosition({-0.03, 0.201801, -0.656399});
        pointLight1->setAmbient({0.05f, 0.05f, 0.05f});
        pointLight1->setDiffuse({1.0f, 0.95f, 0.8f});
        pointLight1->setSpecular({0.01f, 0.01f, 0.01f});
        pointLight1->setConstant(1.0f);
        pointLight1->setLinear(8.09f);
        pointLight1->setQuadratic(0.032f);

        const auto pointLight2 = std::make_shared<Lights::PointLight>();
        pointLight2->setPosition({2.15, 1.2218, -0.656399});
        pointLight2->setAmbient({0.1f, 0.0f, 0.0f});
        pointLight2->setDiffuse({0.88f, 0.0f, 0.00f});
        pointLight2->setSpecular({0.0f, 0.0f, 0.0f});
        pointLight2->setConstant(1.0f);
        pointLight2->setLinear(0.7f);
        pointLight2->setQuadratic(20.8f);

        SceneLights lights;
        lights.spotLights = {spotLight, spotLight2, spotLight3, spotLight4, spotLight5};
        lights.pointLights = {pointLight1, pointLight2};
        return lights;
    }
}
