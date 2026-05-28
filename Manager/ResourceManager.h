#ifndef SNAKE3_RESOURCEMANAGER_H
#define SNAKE3_RESOURCEMANAGER_H

#include <functional>
#include <iostream>
#include <mutex>
#include <unordered_map>
#include <memory>
#include <queue>
#include "TextureManager.h"
#include "ShaderProgram.h"
#include "ShaderRegistry.h"
#include "../Renderer/Opengl/Material/Feature/FogFeature.h"
#include "../Resource/ResourceLoader.h"

// Forward-declare so callsites can use ResourceManager::loadMaterial without
// pulling in nlohmann/json via MaterialLoader.h. Plný include zůstává v .cpp.
namespace Resource { struct MaterialSpec; }

namespace Manager {
        using ModelUtils::Mesh;

        template<class>
        inline constexpr bool always_false = false;

    class ResourceManager final {
    public:
        ResourceManager();

        ~ResourceManager();

        void addTexture(const std::string &name, const std::shared_ptr<TextureManager> &res);

        void replaceTexture(const std::string &name, const std::shared_ptr<TextureManager> &res);

        void addShader(const std::string &name, const std::shared_ptr<ShaderProgram> &res);

        void addModel(const std::string &name, std::shared_ptr<Mesh> &res);

        void addModel(const std::string &name, std::shared_ptr<Animation::AnimationPlayer> res);

        std::shared_ptr<TextureManager> getTexture(const std::string &name) const;
        bool hasTexture(const std::string &name) const;

        std::shared_ptr<ShaderProgram> getShader(const std::string &name) const;

        // B5c: optional handle to the shader registry. App sets it during
        // bootstrap; scenes use it through MaterialBuilder. Will become the
        // primary shader API in B6 once ResourceManager::getShader is retired.
        void setShaderRegistry(std::shared_ptr<ShaderRegistry> registry) { shaderRegistry = std::move(registry); }
        [[nodiscard]] std::shared_ptr<ShaderRegistry> getShaderRegistry() const { return shaderRegistry; }

        // Global fog feature - shared across every basicShader material composition
        // so the F-key toggle propagates to all 3D materials in one mutation.
        // App bootstraps it; RenderManager::toggleFog mutates setEnabled.
        void setFogFeature(std::shared_ptr<Feature::FogFeature> feature) { fogFeature = std::move(feature); }
        [[nodiscard]] std::shared_ptr<Feature::FogFeature> getFogFeature() const { return fogFeature; }

        std::shared_ptr<Mesh> getModel(const std::string &name) const;

        std::shared_ptr<Animation::AnimationPlayer> getAnimationModel(const std::string &name) const;

        void loadAsyncTexture(const std::string &path, const std::string &name, bool albedo, const std::function<void()> &onReady = nullptr);

        // D3.4: JSON-driven material spec loader.
        // Tenký wrapper kolem Resource::loadFromFile - drží callsity bez include
        // nlohmann/json. Vrací MaterialSpec s pre-naplněným builderem (static
        // features) a flagy pro runtime-wired features (lighting/shadow/fog),
        // které musí callsite doplnit z živých objektů.
        [[nodiscard]] Resource::MaterialSpec loadMaterial(const std::string &path) const;

        void loadAsyncShader(
            const std::string &name,
            const std::string &vertexPath,
            const std::string &geometryPath,
            const std::string &fragmentPath,
            const std::function<void()> &onReady
            );

        template<typename T>
        void loadAsyncModel(const std::string &path, const std::string &name, const std::function<void()> &onReady = nullptr) {
            std::lock_guard lock(pendingMutex);
            waitingModels.push_back(name);
            ++loadingCount;

            threads.emplace_back([this, path, name, onReady]() {
                try {
                    if constexpr (std::is_same_v<T, Mesh>) {
                        loader->enqueue(path, [this, name, onReady](const std::vector<std::shared_ptr<Mesh>> &model) {
                            {
                                std::lock_guard guard(pendingMutex);
                                pending.push({name, model, onReady});
                                const auto it = std::find(waitingModels.begin(), waitingModels.end(), name);
                                if (it != waitingModels.end()) waitingModels.erase(it);
                            }
                            --loadingCount;
                        });
                    } else if constexpr (std::is_same_v<T, Animation::AnimationPlayer>) {
                        loader->enqueueAnimation(path, [this, name, onReady](const std::shared_ptr<Animation::AnimationPlayer> &model) {
                            {
                                std::lock_guard guard(pendingMutex);
                                pendingAnim.push({name, model, onReady});
                                const auto it = std::find(waitingModels.begin(), waitingModels.end(), name);
                                if (it != waitingModels.end()) waitingModels.erase(it);
                            }
                            --loadingCount;
                        });
                    } else {
                        static_assert(always_false<T>, "Unsupported type for loadAsyncModel");
                    }
                } catch (const std::exception &e) {
                    std::cerr << "[ResourceManager] Failed to load model " << name << ": " << e.what() << std::endl;
                    --loadingCount;
                }
            });
        }

        void processPending();

        bool isAllLoaded() const;

        void waitForAll();

        bool release();

        void clearTextures();

    protected:
        mutable std::mutex mutex{};
        std::unordered_map<std::string, std::shared_ptr<TextureManager> > texture;
        std::unordered_map<std::string, std::shared_ptr<ShaderProgram> > shader;
        std::unordered_map<std::string, std::shared_ptr<Mesh> > model;
        std::unordered_map<std::string, std::shared_ptr<Animation::AnimationPlayer> > animationModel;
        std::shared_ptr<ShaderRegistry> shaderRegistry;
        std::shared_ptr<Feature::FogFeature> fogFeature;
        std::unique_ptr<Resource::ResourceLoader> loader;

        mutable std::mutex pendingMutex;

        struct PendingItem {
            std::string name;
            std::vector<std::shared_ptr<Mesh>> model;
            std::function<void()> onReady;
        };

        struct PendingAnimation {
            std::string name;
            std::shared_ptr<Animation::AnimationPlayer> model;
            std::function<void()> onReady;
        };

        struct PendingTexture {
            std::string name;
            std::vector<unsigned char> buffer;
            bool albedo;
            std::function<void()> onReady;
        };

        struct PendingShader {
            std::string name;
            std::vector<unsigned char> vertexBuffer;
            std::vector<unsigned char> geometryBuffer;
            std::vector<unsigned char> fragmentBuffer;
            std::function<void()> onReady;
        };

        std::queue<PendingAnimation> pendingAnim;
        std::queue<PendingItem> pending;
        std::queue<PendingTexture> pendingTextures;
        std::queue<PendingShader> pendingShaders;
        std::vector<std::string> waitingModels;
        std::vector<std::thread> threads;

        std::atomic<int> loadingCount{0};
    };
} // Manager

#endif //SNAKE3_RESOURCEMANAGER_H
