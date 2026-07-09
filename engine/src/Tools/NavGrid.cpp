#include <snake3d/Tools/NavGrid.h>

#include <algorithm>
#include <cmath>
#include <queue>

namespace Tools {
    namespace {
        constexpr float kInf = 1e30f;
    }

    void NavGrid::build(const glm::vec2 worldMin, const glm::vec2 worldMax, const float cell,
                        const std::vector<Box> &obstacles, const float agentRadius) {
        origin = worldMin;
        cellSize = std::max(0.25f, cell);
        nx = std::max(1, static_cast<int>(std::ceil((worldMax.x - worldMin.x) / cellSize)));
        ny = std::max(1, static_cast<int>(std::ceil((worldMax.y - worldMin.y) / cellSize)));
        blockedCells.assign(static_cast<size_t>(nx) * ny, 0);

        // Mark every cell whose centre falls inside any obstacle grown by the agent
        // radius (+ half a cell so the agent's body clears the wall, not just its point).
        const float pad = agentRadius + cellSize * 0.5f;
        for (const auto &b : obstacles) {
            const glm::vec2 mn = b.min - glm::vec2(pad);
            const glm::vec2 mx = b.max + glm::vec2(pad);
            int x0, y0, x1, y1;
            worldToCell(mn, x0, y0);
            worldToCell(mx, x1, y1);
            x0 = std::clamp(x0, 0, nx - 1); y0 = std::clamp(y0, 0, ny - 1);
            x1 = std::clamp(x1, 0, nx - 1); y1 = std::clamp(y1, 0, ny - 1);
            for (int cy = y0; cy <= y1; ++cy)
                for (int cx = x0; cx <= x1; ++cx)
                    blockedCells[idx(cx, cy)] = 1;
        }
    }

    glm::vec2 NavGrid::cellCenter(const int cx, const int cy) const {
        return origin + glm::vec2((cx + 0.5f) * cellSize, (cy + 0.5f) * cellSize);
    }

    void NavGrid::worldToCell(const glm::vec2 w, int &cx, int &cy) const {
        cx = static_cast<int>(std::floor((w.x - origin.x) / cellSize));
        cy = static_cast<int>(std::floor((w.y - origin.y) / cellSize));
    }

    bool NavGrid::blocked(const glm::vec2 world) const {
        int cx, cy;
        worldToCell(world, cx, cy);
        return cellBlocked(cx, cy);
    }

    bool NavGrid::lineClear(const glm::vec2 a, const glm::vec2 b) const {
        // Sample the segment at half-cell steps; clear iff no sample lands in a blocked cell.
        const float d = glm::length(b - a);
        const int steps = std::max(1, static_cast<int>(d / (cellSize * 0.5f)));
        for (int i = 0; i <= steps; ++i) {
            const glm::vec2 p = a + (b - a) * (static_cast<float>(i) / steps);
            if (blocked(p)) return false;
        }
        return true;
    }

    bool NavGrid::nearestFree(int &cx, int &cy) const {
        if (!cellBlocked(cx, cy)) return true;
        for (int r = 1; r <= 6; ++r) {
            for (int dy = -r; dy <= r; ++dy) {
                for (int dx = -r; dx <= r; ++dx) {
                    if (std::abs(dx) != r && std::abs(dy) != r) continue; // ring only
                    const int x = cx + dx, y = cy + dy;
                    if (!cellBlocked(x, y)) { cx = x; cy = y; return true; }
                }
            }
        }
        return false;
    }

    std::vector<glm::vec3> NavGrid::findPath(const glm::vec3 from, const glm::vec3 to) const {
        std::vector<glm::vec3> out;
        if (!ready()) return out;

        int sx, sy, gx, gy;
        worldToCell(glm::vec2(from), sx, sy);
        worldToCell(glm::vec2(to), gx, gy);
        if (!nearestFree(sx, sy) || !nearestFree(gx, gy)) return out;
        if (sx == gx && sy == gy) return out; // already there

        const int n = nx * ny;
        std::vector<float> g(n, kInf);
        std::vector<int> came(n, -1);
        std::vector<std::uint8_t> closed(n, 0);

        using Node = std::pair<float, int>; // (f, cell)
        std::priority_queue<Node, std::vector<Node>, std::greater<>> open;

        auto h = [&](const int cx, const int cy) {
            const auto dx = static_cast<float>(gx - cx), dy = static_cast<float>(gy - cy);
            return std::sqrt(dx * dx + dy * dy);
        };

        const int start = idx(sx, sy), goal = idx(gx, gy);
        g[start] = 0.0f;
        open.emplace(h(sx, sy), start);

        constexpr int dxs[8] = {1, -1, 0, 0, 1, 1, -1, -1};
        constexpr int dys[8] = {0, 0, 1, -1, 1, -1, 1, -1};

        bool found = false;
        while (!open.empty()) {
            const int cur = open.top().second;
            open.pop();
            if (closed[cur]) continue;
            closed[cur] = 1;
            if (cur == goal) { found = true; break; }
            const int cx = cur % nx, cy = cur / nx;
            for (int k = 0; k < 8; ++k) {
                const int nxc = cx + dxs[k], nyc = cy + dys[k];
                if (cellBlocked(nxc, nyc)) continue;
                if (k >= 4) { // diagonal: don't cut blocked corners
                    if (cellBlocked(cx + dxs[k], cy) || cellBlocked(cx, cy + dys[k])) continue;
                }
                const int ni = idx(nxc, nyc);
                if (closed[ni]) continue;
                const float step = (k < 4) ? 1.0f : 1.41421356f;
                const float ng = g[cur] + step;
                if (ng < g[ni]) {
                    g[ni] = ng;
                    came[ni] = cur;
                    open.emplace(ng + h(nxc, nyc), ni);
                }
            }
        }
        if (!found) return out;

        // Reconstruct cell path (goal -> start), then reverse.
        std::vector<int> cells;
        for (int c = goal; c != -1; c = came[c]) cells.push_back(c);
        std::ranges::reverse(cells);

        // String-pull: keep a waypoint only when the straight line from the last kept
        // point to the NEXT candidate is no longer clear, removing redundant zig-zag.
        std::vector<glm::vec2> pts;
        pts.reserve(cells.size());
        for (const int c : cells) pts.push_back(cellCenter(c % nx, c / nx));

        std::vector<glm::vec2> smooth;
        smooth.push_back(pts.front());
        size_t anchor = 0;
        for (size_t i = 2; i < pts.size(); ++i) {
            if (!lineClear(pts[anchor], pts[i])) {
                smooth.push_back(pts[i - 1]);
                anchor = i - 1;
            }
        }
        smooth.push_back(pts.back());

        out.reserve(smooth.size());
        for (const auto &p : smooth) out.emplace_back(p.x, p.y, from.z);
        return out;
    }
} // namespace Tools
