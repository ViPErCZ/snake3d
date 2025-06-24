#include "ResourceManager.h"

namespace Manager {
    ResourceManager::~ResourceManager() {
        Release();
    }

    bool ResourceManager::Release() {
        animationModel.clear();
        model.clear();
        texture.clear();

        return true;
    }

    void ResourceManager::addTexture(const string& name, const shared_ptr<TextureManager>& res) {
        std::unique_lock lock(mutex);
        if (const auto [fst, snd] = texture.emplace(name, res); !snd) {
            throw invalid_argument("Failed to add texture " + name + ", already contains.");
        }
    }

    TextureManager* ResourceManager::getTexture(const string &name) const {
        std::unique_lock lock(mutex);
        try {
            return texture.at(name).get();
        } catch (...) {
            throw invalid_argument("No such resource called " + name);
        }
    }

    void ResourceManager::addModel(const string &name, std::shared_ptr<ObjItem> res) {
        std::unique_lock lock(mutex);
        if (const auto [fst, snd] = model.emplace(name, std::move(res)); !snd) {
            throw invalid_argument("Failed to add model " + name + ", already contains.");
        }
    }

    void ResourceManager::addModel(const string &name, std::shared_ptr<AnimationModel> res) {
        std::unique_lock lock(mutex);
        if (const auto [fst, snd] = animationModel.emplace(name, std::move(res)); !snd) {
            throw invalid_argument("Failed to add model " + name + ", already contains.");
        }
    }

    ObjItem* ResourceManager::getModel(const string &name) const {
        std::unique_lock lock(mutex);
        try {
            return model.at(name).get();
        } catch (...) {
            throw invalid_argument("No such resource called " + name);
        }
    }

    AnimationModel* ResourceManager::getAnimationModel(const string &name) const {
        std::unique_lock lock(mutex);
        try {
            return animationModel.at(name).get();
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

    ShaderManager *ResourceManager::getShader(const string &name) const {
        std::unique_lock lock(mutex);
        try {
            return shader.at(name).get();
        } catch (...) {
            throw invalid_argument("No such resource called " + name);
        }
    }

} // Manager