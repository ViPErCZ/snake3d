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
#include "../Resource/ResourceLoader.h"

using namespace std;
using namespace Resource;
using namespace Animations;

namespace Manager {
        template<class>
        inline constexpr bool always_false = false;

    class ResourceManager final {
    public:
        ResourceManager();

        ~ResourceManager();

        void addTexture(const string &name, const shared_ptr<TextureManager> &res);

        void replaceTexture(const string &name, const shared_ptr<TextureManager> &res);

        void addShader(const string &name, const shared_ptr<ShaderProgram> &res);

        void addModel(const string &name, shared_ptr<Mesh> &res);

        void addModel(const string &name, shared_ptr<AnimationPlayer> res);

        shared_ptr<TextureManager> getTexture(const string &name) const;
        bool hasTexture(const string &name) const;

        shared_ptr<ShaderProgram> getShader(const string &name) const;

        // B5c: optional handle to the shader registry. App sets it during
        // bootstrap; scenes use it through MaterialBuilder. Will become the
        // primary shader API in B6 once ResourceManager::getShader is retired.
        void setShaderRegistry(shared_ptr<ShaderRegistry> registry) { shaderRegistry = std::move(registry); }
        [[nodiscard]] shared_ptr<ShaderRegistry> getShaderRegistry() const { return shaderRegistry; }

        shared_ptr<Mesh> getModel(const string &name) const;

        shared_ptr<AnimationPlayer> getAnimationModel(const string &name) const;

        void loadAsyncTexture(const string &path, const string &name, bool albedo, const function<void()> &onReady = nullptr);

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
                    } else if constexpr (std::is_same_v<T, AnimationPlayer>) {
                        loader->enqueueAnimation(path, [this, name, onReady](const std::shared_ptr<AnimationPlayer> &model) {
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
        std::unordered_map<std::string, std::shared_ptr<AnimationPlayer> > animationModel;
        std::shared_ptr<ShaderRegistry> shaderRegistry;
        std::unique_ptr<ResourceLoader> loader;

        mutable std::mutex pendingMutex;

        struct PendingItem {
            std::string name;
            std::vector<std::shared_ptr<Mesh>> model;
            std::function<void()> onReady;
        };

        struct PendingAnimation {
            std::string name;
            std::shared_ptr<AnimationPlayer> model;
            std::function<void()> onReady;
        };

        struct PendingTexture {
            std::string name;
            vector<unsigned char> buffer;
            bool albedo;
            std::function<void()> onReady;
        };

        struct PendingShader {
            std::string name;
            vector<unsigned char> vertexBuffer;
            vector<unsigned char> geometryBuffer;
            vector<unsigned char> fragmentBuffer;
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
