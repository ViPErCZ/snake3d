#include "ResourceLoader.h"
#include <utility>
#include "AnimLoader.h"
#include "ObjModelLoader.h"
#include "ShaderLoader.h"
#include "TextureLoader.h"

namespace Resource {
    ResourceLoader::ResourceLoader() {
        worker = std::thread([this]() { workerLoop(); });
        std::thread([this]() { workerLoopAnim(); }).detach();
        std::thread([this]() { workerLoopTexture(); }).detach();
        std::thread([this]() { workerLoopShader(); }).detach();
    }

    ResourceLoader::~ResourceLoader() {
        stop();
    }

    void ResourceLoader::stop() {
        {
            std::lock_guard lock(queueMutex);
            running = false;
        }
        cv.notify_all(); // probudí worker, pokud spí
        if (worker.joinable())
            worker.join();
    }

    void ResourceLoader::enqueue(const std::filesystem::path& path, Callback callback) {
        {
            std::lock_guard lock(queueMutex);
            jobs.push({path, std::move(callback)});
        }
        cv.notify_one();
    }

    void ResourceLoader::enqueueAnimation(const std::filesystem::path &path, AnimCallback callback) {
        {
            std::lock_guard lock(queueMutex);
            animJobs.push({path, std::move(callback)});
        }
        cv.notify_all();
    }

    void ResourceLoader::enqueueTexture(const std::filesystem::path &path, const bool albedo, TextureCallback callback) {
        {
            std::lock_guard lock(queueMutex);
            textureJobs.push({path, albedo, std::move(callback)});
        }
        cv.notify_all();
    }

    void ResourceLoader::enqueueShader(
        const std::filesystem::path &vertexPath,
        const std::filesystem::path &geometryPath,
        const std::filesystem::path &fragmentPath,
        ShaderCallback callback) {
        {
            std::lock_guard lock(queueMutex);
            shaderJobs.push({vertexPath, geometryPath, fragmentPath, std::move(callback)});
        }
        cv.notify_all();
    }

    void ResourceLoader::workerLoop() {
        while (true) {
            Job job;

            {
                std::unique_lock lock(queueMutex);
                cv.wait(lock, [this]() { return !jobs.empty() || !running; });

                if (!running && jobs.empty()) {
                    break;
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                job = jobs.front();
                jobs.pop();
            }

            loadSemaphore.acquire();

            // Spustíme job asynchronně
            std::thread([this, job]() {
                try {
                    const auto obj = ObjModelLoader::loadObj(job.path);
                    job.callback(obj);
                } catch (const std::exception &e) {
                    std::cerr << "[ResourceLoader] Failed to load " << job.path << ": " << e.what() << "\n";
                }
                loadSemaphore.release();
                cv.notify_one();
            }).detach();
        }
    }

    void ResourceLoader::workerLoopAnim() {
        while (true) {
            AnimJob job;

            {
                std::unique_lock lock(queueMutex);
                cv.wait(lock, [this]() { return !animJobs.empty() || !running; });

                if (!running && animJobs.empty()) {
                    break;
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                job = animJobs.front();
                animJobs.pop();
            }

            loadSemaphore.acquire();
            std::thread([this, job]() {
                try {
                    const auto anim = AnimLoader::loadObj(job.path);
                    job.callback(anim);
                } catch (const std::exception &e) {
                    std::cerr << "[ResourceLoader] Failed to load ANIM " << job.path << ": " << e.what() << "\n";
                }
                loadSemaphore.release();
                cv.notify_one();
            }).detach();
        }
    }

    void ResourceLoader::workerLoopTexture() {
        while (true) {
            TextureJob job;

            {
                std::unique_lock lock(queueMutex);
                cv.wait(lock, [this]() { return !textureJobs.empty() || !running; });

                if (!running && textureJobs.empty()) {
                    break;
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                job = textureJobs.front();
                textureJobs.pop();
            }

            loadSemaphore.acquire();
            std::thread([this, job]() {
                try {
                    const vector<unsigned char> data = TextureLoader::loadTextureToBuffer(job.path);
                    job.callback(data, job.albedo);
                } catch (const std::exception &e) {
                    std::cerr << "[ResourceLoader] Failed to load TEXTURE " << job.path << ": " << e.what() << "\n";
                }
                loadSemaphore.release();
                cv.notify_one();
            }).detach();
        }
    }

    void ResourceLoader::workerLoopShader() {
        while (true) {
            ShaderJob job;

            {
                std::unique_lock lock(queueMutex);
                cv.wait(lock, [this]() { return !shaderJobs.empty() || !running; });

                if (!running && shaderJobs.empty()) {
                    break;
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                job = shaderJobs.front();
                shaderJobs.pop();
            }

            loadSemaphore.acquire();
            std::thread([this, job]() {
                try {
                    if (job.geometryPath.empty()) {
                        const fvShader shader = ShaderLoader::loadShaderToBuffer(job.vertexPath, job.fragmentPath);
                        const vector<unsigned char> geom;
                        job.callback(shader.vertex, shader.fragment, geom);
                    } else {
                        const fgvShader shader = ShaderLoader::loadShaderToBuffer(job.vertexPath, job.geometryPath, job.fragmentPath);
                        job.callback(shader.vertex, shader.fragment, shader.geometry);
                    }
                } catch (const std::exception &e) {
                    std::cerr << "[ResourceLoader] Failed to load SHADER " << job.vertexPath << ": " << e.what() << "\n";
                }
                loadSemaphore.release();
                cv.notify_one();
            }).detach();
        }
    }
} // Resource