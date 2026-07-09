#ifndef SNAKE3_NAVGRID_H
#define SNAKE3_NAVGRID_H

#include <cstdint>
#include <vector>

#include <glm/glm.hpp>

namespace Tools {
    // A uniform occupancy grid + A* pathfinder for a flat (XY-plane) world whose
    // obstacles are axis-aligned boxes (walls, crates, building footprints). Decoupled
    // from the physics/collision system on purpose: you hand it plain 2D box footprints,
    // it gives you a smoothed list of world waypoints. Suits a Z-up world where agents
    // walk the ground plane; the Z of inputs is ignored and passed straight back out.
    //
    // Typical use: build() once per map from the static world AABBs, then findPath()
    // per agent on a timer. Cheap enough for a few dozen agents at ~1 m cells.
    class NavGrid {
    public:
        struct Box { glm::vec2 min{0.0f}; glm::vec2 max{0.0f}; }; // XY footprint

        NavGrid() = default;

        // Rasterise the obstacles (each grown by agentRadius) into a blocked-cell grid
        // spanning [worldMin, worldMax]. cell is the square cell size in world units.
        void build(glm::vec2 worldMin, glm::vec2 worldMax, float cell,
                   const std::vector<Box> &obstacles, float agentRadius);

        [[nodiscard]] bool ready() const { return nx > 0 && ny > 0; }

        // Is the world point inside a blocked cell (or outside the grid)?
        [[nodiscard]] bool blocked(glm::vec2 world) const;

        // Straight-line walkability between two world points (grid raycast). Used for
        // path smoothing and by callers to decide "go straight" vs "follow the path".
        [[nodiscard]] bool lineClear(glm::vec2 a, glm::vec2 b) const;

        // Smoothed list of world waypoints from -> to (cell centres, string-pulled).
        // Empty if unreachable. The returned vec3 carries `from.z` in z (XY is the path).
        [[nodiscard]] std::vector<glm::vec3> findPath(glm::vec3 from, glm::vec3 to) const;

    private:
        [[nodiscard]] int idx(int cx, int cy) const { return cy * nx + cx; }
        [[nodiscard]] bool inBounds(int cx, int cy) const { return cx >= 0 && cy >= 0 && cx < nx && cy < ny; }
        [[nodiscard]] bool cellBlocked(int cx, int cy) const { return !inBounds(cx, cy) || blockedCells[idx(cx, cy)]; }
        [[nodiscard]] glm::vec2 cellCenter(int cx, int cy) const;
        void worldToCell(glm::vec2 w, int &cx, int &cy) const;
        // Nearest non-blocked cell to (cx,cy) within a small radius (for clamping
        // endpoints that fall inside/!near an obstacle). Returns false if none found.
        bool nearestFree(int &cx, int &cy) const;

        glm::vec2 origin{0.0f};   // worldMin
        float cellSize{1.0f};
        int nx{0}, ny{0};
        std::vector<std::uint8_t> blockedCells;
    };
} // namespace Tools

#endif // SNAKE3_NAVGRID_H
