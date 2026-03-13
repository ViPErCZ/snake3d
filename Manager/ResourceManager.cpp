#include "ResourceManager.h"

#include "../Resource/ShaderLoader.h"
#include "../Resource/TextureLoader.h"

namespace Manager {
    ResourceManager::ResourceManager() {
        std::unique_lock lock(mutex);
        loader = make_unique<ResourceLoader>();
    }

    ResourceManager::~ResourceManager() {
        release();
    }

    bool ResourceManager::release() {
        std::unique_lock lock(mutex);

        if (loader) {
            loader->stop();
            loader.reset();
        }

        for (auto &t: threads) {
            if (t.joinable()) {
                t.join();
            }
        }
        threads.clear(); {
            std::lock_guard guard(pendingMutex);
            while (!pending.empty()) pending.pop();
            waitingModels.clear();
        }

        animationModel.clear();
        model.clear();
        texture.clear();
        shader.clear();

        return true;
    }

    void ResourceManager::clearTextures() {
        for (auto it = texture.begin(); it != texture.end();) {
            if (it->first != "depth") {
                it = texture.erase(it);
            } else {
                ++it;
            }
        }
    }

    void ResourceManager::addTexture(const string &name, const shared_ptr<TextureManager> &res) {
        std::unique_lock lock(mutex);
        if (const auto [fst, snd] = texture.emplace(name, res); !snd) {
            throw invalid_argument("Failed to add texture " + name + ", already contains.");
        }
    }

    void ResourceManager::replaceTexture(const string &name, const shared_ptr<TextureManager> &res) {
        std::unique_lock lock(mutex);
        if (const auto [fst, snd] = texture.emplace(name, res); !snd) {
            texture.erase(texture.find(name));
            if (const auto [fst, snd] = texture.emplace(name, res); !snd) {
                throw invalid_argument("Failed to add texture " + name + ", already contains.");
            }
        }
    }

    std::shared_ptr<TextureManager> ResourceManager::getTexture(const string &name) const {
        std::unique_lock lock(mutex);
        try {
            return texture.at(name);
        } catch (...) {
            throw invalid_argument("No such resource called " + name);
        }
    }

    bool ResourceManager::hasTexture(const string &name) const {
        std::unique_lock lock(mutex);
        return texture.contains(name);
    }

    void ResourceManager::addModel(const string &name, std::shared_ptr<Mesh> &res) {
        std::unique_lock lock(mutex);
        if (const auto [fst, snd] = model.emplace(name, res); !snd) {
            throw invalid_argument("Failed to add model " + name + ", already contains.");
        }
    }

    void ResourceManager::addModel(const string &name, std::shared_ptr<AnimationPlayer> res) {
        std::unique_lock lock(mutex);
        if (const auto [fst, snd] = animationModel.emplace(name, std::move(res)); !snd) {
            throw invalid_argument("Failed to add model " + name + ", already contains.");
        }
    }

    shared_ptr<Mesh> ResourceManager::getModel(const string &name) const {
        std::unique_lock lock(mutex);
        try {
            return model.at(name);
        } catch (...) {
            throw invalid_argument("No such resource called " + name);
        }
    }

    shared_ptr<AnimationPlayer> ResourceManager::getAnimationModel(const string &name) const {
        std::unique_lock lock(mutex);
        try {
            return animationModel.at(name);
        } catch (...) {
            throw invalid_argument("No such resource called " + name);
        }
    }

    void ResourceManager::addShader(const string &name, const shared_ptr<ShaderManager> &res) {
        std::unique_lock lock(mutex);
        if (const auto [fst, snd] = shader.emplace(name, res); !snd) {
            throw invalid_argument("Failed to add baseShader " + name + ", already contains.");
        }
    }

    std::shared_ptr<ShaderManager> ResourceManager::getShader(const string &name) const {
        std::unique_lock lock(mutex);
        try {
            return shader.at(name);
        } catch (...) {
            throw invalid_argument("No such resource called " + name);
        }
    }

    void ResourceManager::loadAsyncTexture(
        const std::string &path,
        const std::string &name,
        const bool albedo,
        const std::function<void()> &onReady) {
        std::unique_lock lock(mutex);
        waitingModels.push_back(name);
        ++loadingCount;

        threads.emplace_back([this, path, name, albedo, onReady]() {
            loader->enqueueTexture(path, albedo,
                                   [this, name, onReady](const vector<unsigned char> &buffer, const bool isAlbedo) {
                                       {
                                           std::lock_guard guard(pendingMutex);
                                           pendingTextures.push({name, buffer, isAlbedo, onReady});
                                           const auto it = std::find(waitingModels.begin(), waitingModels.end(), name);
                                           if (it != waitingModels.end()) waitingModels.erase(it);
                                       }
                                       --loadingCount;
                                   });
        });
    }

    void ResourceManager::loadAsyncShader(
        const std::string &name,
        const std::string &vertexPath,
        const std::string &geometryPath,
        const std::string &fragmentPath,
        const std::function<void()> &onReady) {
        std::unique_lock lock(mutex);
        waitingModels.push_back(name);
        ++loadingCount;

        threads.emplace_back([this, vertexPath, geometryPath, fragmentPath, name, onReady]() {
            loader->enqueueShader(vertexPath, geometryPath, fragmentPath,
                                  [this, name, onReady](const vector<unsigned char> &vertexBuffer,
                                                        const vector<unsigned char> &fragmentBuffer,
                                                        const vector<unsigned char> &geometryBuffer
                                                        ) {
                                      {
                                          std::lock_guard guard(pendingMutex);
                                          pendingShaders.push({
                                              name, vertexBuffer, geometryBuffer, fragmentBuffer, onReady
                                          });
                                          const auto it = std::find(waitingModels.begin(), waitingModels.end(), name);
                                          if (it != waitingModels.end()) waitingModels.erase(it);
                                      }
                                      --loadingCount;
                                  });
        });
    }

    void ResourceManager::processPending() {
        std::lock_guard lock(pendingMutex);

        while (!pending.empty()) {
            auto p = pending.front();
            pending.pop();

            // Nahrání do GPU atd. zde:
            if (p.model.size() == 1) {
                addModel(p.name, p.model[0]);
            } else {
                int i = 0;
                for (auto &m : p.model) {
                    addModel(p.name + "_" + std::to_string(i), m);
                    // m->
                    i++;
                }
            }

            if (p.onReady) p.onReady();
        }

        while (!pendingAnim.empty()) {
            auto p = pendingAnim.front();
            pendingAnim.pop();

            addModel(p.name, p.model);
            if (p.onReady) p.onReady();
        }

        while (!pendingTextures.empty()) {
            auto p = pendingTextures.front();
            pendingTextures.pop();

            // tady mozna misto v p.Textures mit jen buffer a ten rovnou nahrat do GPU uz tady ????
            auto texture = make_shared<TextureManager>();
            texture->addTexture(TextureLoader::bindFromBuffer(p.buffer, p.albedo));
            addTexture(p.name, texture);
            p.buffer.clear();
            if (p.onReady) p.onReady();
        }

        while (!pendingShaders.empty()) {
            auto p = pendingShaders.front();
            pendingShaders.pop();

            string vertexBuffer(p.vertexBuffer.begin(), p.vertexBuffer.end());
            string geometryBuffer(p.geometryBuffer.begin(), p.geometryBuffer.end());
            string fragmentBuffer(p.fragmentBuffer.begin(), p.fragmentBuffer.end());

            if (geometryBuffer.empty()) {
                auto shader = make_shared<ShaderManager>(ShaderLoader::bindFromBuffer(vertexBuffer, fragmentBuffer));
                addShader(p.name, shader);
            } else {
                auto shader = make_shared<ShaderManager>(ShaderLoader::bindFromBuffer(vertexBuffer, geometryBuffer, fragmentBuffer));
                addShader(p.name, shader);
            }
            p.vertexBuffer.clear();
            p.geometryBuffer.clear();
            p.fragmentBuffer.clear();
            if (p.onReady) p.onReady();
        }
    }

    bool ResourceManager::isAllLoaded() const {
        std::lock_guard<std::mutex> lock(pendingMutex);
        return waitingModels.empty() && pending.empty() && loadingCount.load() == 0;
    }

    void ResourceManager::waitForAll() {
        // Počká na všechna vlákna
        for (auto &t: threads) {
            if (t.joinable()) t.join();
        }
        threads.clear();

        // Zpracuj zbytek pending modelů
        processPending();
    }
} // Manager
