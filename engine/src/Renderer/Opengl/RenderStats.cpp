#include <snake3d/Renderer/Opengl/RenderStats.h>

#include <atomic>

namespace Renderer {
    std::atomic<int> RenderStats::drawCallsThisFrame{0};
    int RenderStats::drawCallsLastFrame = 0;

    float RenderStats::mainPassMsCpu = 0.0f;
    float RenderStats::mainPassMsCpuOnly = 0.0f;
    float RenderStats::updateMs = 0.0f;
    float RenderStats::physicsMs = 0.0f;
    float RenderStats::renderMs = 0.0f;
    float RenderStats::swapMs = 0.0f;
    int RenderStats::collidersStatic = 0;
    int RenderStats::collidersDynamic = 0;
    int RenderStats::pairsTested = 0;
    float RenderStats::physAabbMs = 0.0f;
    float RenderStats::physPairsMs = 0.0f;
    float RenderStats::updateNetMs = 0.0f;
    float RenderStats::updateSceneMs = 0.0f;
    float RenderStats::updateHudMs = 0.0f;

    int RenderStats::drawsPerPassThisFrame[static_cast<int>(RenderPass::Count)] = {0};
    int RenderStats::drawsPerPassLastFrame[static_cast<int>(RenderPass::Count)] = {0};

    std::unordered_set<unsigned int> RenderStats::uniqueProgramsThisFrame;
    int RenderStats::uniqueProgramsLastFrame = 0;

    RenderPass RenderStats::currentPass = RenderPass::Main;

    void RenderStats::newFrame() {
        drawCallsLastFrame = drawCallsThisFrame.exchange(0);
        for (int i = 0; i < static_cast<int>(RenderPass::Count); ++i) {
            drawsPerPassLastFrame[i] = drawsPerPassThisFrame[i];
            drawsPerPassThisFrame[i] = 0;
        }
        uniqueProgramsLastFrame = static_cast<int>(uniqueProgramsThisFrame.size());
        uniqueProgramsThisFrame.clear();
        currentPass = RenderPass::Main;
    }

    void RenderStats::countDraw() {
        drawCallsThisFrame.fetch_add(1, std::memory_order_relaxed);
        ++drawsPerPassThisFrame[static_cast<int>(currentPass)];
    }

    void RenderStats::countProgramUse(const unsigned int programId) {
        uniqueProgramsThisFrame.insert(programId);
    }

    void RenderStats::setPass(const RenderPass pass) {
        currentPass = pass;
    }
} // Renderer
