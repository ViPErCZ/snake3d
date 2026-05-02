#ifndef SNAKE3_RESOURCELOADER_H
#define SNAKE3_RESOURCELOADER_H

#include <thread>
#include <mutex>
#include <queue>
#include <functional>
#include <atomic>
#include <filesystem>
#include <memory>
#include <condition_variable>
#include "../Manager/TextureManager.h"
#include "../Renderer/Opengl/Model/Standard/Animation/AnimationPlayer.h"

using namespace Manager;
using namespace Animations;

namespace Resource {
    class ResourceLoader {
    public:
        using Callback = std::function<void(std::vector<std::shared_ptr<Mesh>>)>;
        using AnimCallback = std::function<void(std::shared_ptr<AnimationPlayer>)>;
        using TextureCallback = std::function<void(vector<unsigned char>, bool albedo)>;
        using ShaderCallback = std::function<void(vector<unsigned char>, vector<unsigned char>, vector<unsigned char>)>;

        ResourceLoader();
        ~ResourceLoader();

        void enqueue(const std::filesystem::path& path, Callback callback);
        void enqueueAnimation(const std::filesystem::path &path, AnimCallback callback);
        void enqueueTexture(const std::filesystem::path &path, bool albedo, TextureCallback callback);
        void enqueueShader(
            const std::filesystem::path &vertexPath,
            const std::filesystem::path &geometryPath,
            const std::filesystem::path &fragmentPath,
            ShaderCallback callback
            );
        void stop();
    private:
        static constexpr int MAX_CONCURRENT_LOADS = 1;
        struct Job {
            std::filesystem::path path;
            Callback callback;
        };
        struct AnimJob {
            std::filesystem::path path;
            AnimCallback callback;
        };
        struct TextureJob {
            std::filesystem::path path;
            bool albedo;
            TextureCallback callback;
        };
        struct ShaderJob {
            std::filesystem::path vertexPath;
            std::filesystem::path geometryPath;
            std::filesystem::path fragmentPath;
            ShaderCallback callback;
        };

        std::queue<Job> jobs;
        std::queue<AnimJob> animJobs;
        std::queue<TextureJob> textureJobs;
        std::queue<ShaderJob> shaderJobs;
        mutable std::mutex queueMutex;
        std::thread modelWorker;
        std::thread animWorker;
        std::thread textureWorker;
        std::thread shaderWorker;
        std::atomic<bool> running = true;
        std::condition_variable cv;
        std::counting_semaphore<MAX_CONCURRENT_LOADS> loadSemaphore{MAX_CONCURRENT_LOADS};

        void workerLoop();
        void workerLoopAnim();
        void workerLoopTexture();
        void workerLoopShader();
    };
} // Resource

#endif //SNAKE3_RESOURCELOADER_H
