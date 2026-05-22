#ifndef SNAKE3_RENDERSTATS_H
#define SNAKE3_RENDERSTATS_H

#include <atomic>
#include <unordered_set>

namespace Renderer {
    enum class RenderPass {
        Main = 0,    // standardní forward pass (default)
        Shadow = 1,  // CSM cascade pass
        Reflection = 2,  // planar reflection pass
        Bloom = 3,   // post-process bloom
        Count = 4
    };

    // Lightweight per-frame counters pro diagnostiku rendering pipeline.
    // Inkrementované call sites (StandardMesh::render etc.); resetované
    // jednou per frame z RenderManager::render. ImGui Engine panel ukazuje
    // lastFrame snapshoty.
    struct RenderStats {
        static std::atomic<int> drawCallsThisFrame;
        static int drawCallsLastFrame;

        // Main pass timing v ms. Měřeno chrono okolo main pass loop + glFinish
        // na konci pro GPU sync. Pomáhá rozdělit CPU vs GPU bound diagnostiku:
        //   - mainPassMsCpu měří CPU + sync na GPU (glFinish stall).
        //   - mainPassMsCpuOnly měří jen CPU draw call setup (chrono mezi
        //     start/end main pass bez glFinish).
        // mainPassMsCpu - mainPassMsCpuOnly ≈ GPU work time.
        static float mainPassMsCpu;        // total wall-clock incl GPU sync
        static float mainPassMsCpuOnly;    // CPU only (chrono diff before glFinish)

        // Frame breakdown (CPU side) - per App::run phase. Nastavované z App.
        static float updateMs;   // mainScene->update() scene graph traversal
        static float physicsMs;  // mainScene->physics() collision step
        static float renderMs;   // mainScene->render() (incl ImGui begin/end)
        static float swapMs;     // glfwSwapBuffers + glfwPollEvents

        // Physics breakdown
        static int collidersStatic;    // počet static colliderů v scéně
        static int collidersDynamic;   // počet dynamic colliderů
        static int pairsTested;        // pairs po static/dynamic split filter
        static float physAabbMs;       // AABB build phase
        static float physPairsMs;      // pair iteration phase

        // Update breakdown (MainScene::update sections)
        static float updateNetMs;      // netSession.tick + remote authoritative
        static float updateSceneMs;    // Scene::update (incl. sub-scenes + worldMatrix)
        static float updateHudMs;      // HUD radar + eatManager checkPlace

        // Per-pass split. RenderManager sets currentPass přes setPass()
        // při entry každého pass, countDraw() inkrementuje correct slot.
        static int drawsPerPassThisFrame[static_cast<int>(RenderPass::Count)];
        static int drawsPerPassLastFrame[static_cast<int>(RenderPass::Count)];

        // Unique shader programů použitých per frame (shader switches indicator).
        static std::unordered_set<unsigned int> uniqueProgramsThisFrame;
        static int uniqueProgramsLastFrame;

        // Reset frame counter, copy this→last. Volat na začátku frame.
        static void newFrame();

        // Increment draw call (do current pass slot).
        static void countDraw();

        // Volat z MaterialInstance::bind / ShaderProgram::use - track unique IDs.
        static void countProgramUse(unsigned int programId);

        // Pass setter - volaný z RenderManager::render pře entry každého pass.
        static void setPass(RenderPass pass);
        static RenderPass currentPass;
    };
} // Renderer

#endif //SNAKE3_RENDERSTATS_H
