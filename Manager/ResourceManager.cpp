#include "ResourceManager.h"

namespace Manager {
    ResourceManager::~ResourceManager() {
        Release();
    }

    bool ResourceManager::Release() {
        model.clear();
        texture.clear();

        return true;
    }

    void ResourceManager::addTexture(const string& name, const shared_ptr<TextureManager>& res) {
        std::unique_lock lock(mutex);
        const auto result = texture.emplace(name, res);
        if (!result.second) {
            throw invalid_argument("Failed to add texture " + name + ", already contains.");
        }
    }

    TextureManager* ResourceManager::getTexture(const string &name) {
        std::unique_lock lock(mutex);
        try {
            return texture.at(name).get();
        } catch (...) {
            throw invalid_argument("No such resource called " + name);
        }
    }

    void ResourceManager::addModel(const string &name, std::shared_ptr<ObjItem> res) {
        std::unique_lock lock(mutex);
        const auto result = model.emplace(name, std::move(res));
        if (!result.second) {
            throw invalid_argument("Failed to add model " + name + ", already contains.");
        }
    }

    ObjItem* ResourceManager::getModel(const string &name) {
        std::unique_lock lock(mutex);
        try {
            return model.at(name).get();
        } catch (...) {
            throw invalid_argument("No such resource called " + name);
        }
    }

    void ResourceManager::addShader(const string &name, const shared_ptr<ShaderManager> &res) {
        std::unique_lock lock(mutex);
        const auto result = shader.emplace(name, res);
        if (!result.second) {
            throw invalid_argument("Failed to add shader " + name + ", already contains.");
        }
    }

    ShaderManager *ResourceManager::getShader(const string &name) {
        std::unique_lock lock(mutex);
        try {
            return shader.at(name).get();
        } catch (...) {
            throw invalid_argument("No such resource called " + name);
        }
    }

} // Manager