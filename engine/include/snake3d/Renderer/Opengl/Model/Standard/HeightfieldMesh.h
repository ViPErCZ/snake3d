#ifndef SNAKE3_HEIGHTFIELDMESH_H
#define SNAKE3_HEIGHTFIELDMESH_H

#include <functional>
#include <vector>

#include <snake3d/Renderer/Opengl/Model/Standard/StandardMesh.h>

namespace Model {
    // A tessellated ground grid built from a game-supplied HEIGHT FUNCTION: the engine
    // owns the mechanics (grid tessellation, smooth normals, tangents, height queries),
    // the game owns the design (whatever field it wants - lake basins, hills, noise).
    //
    // The grid spans [-halfSpan, +halfSpan]^2 in XZ with gridCells cells per side and
    // y = height(x, z) at every vertex. Smooth per-vertex normals come from central
    // differences of the field; texUV tiles once per cell (world-tile UV, the usual
    // terrain-shader convention).
    //
    // Two DIFFERENT height queries - pick deliberately:
    //  - heightAt(x, z): the analytic FIELD (the stored function). Use for build-time
    //    queries against the design itself (water levels, collider placement).
    //  - heightAtMesh(x, z): the RENDERED surface, interpolated across the actual mesh
    //    triangles (same cell diagonal as the index buffer). Between grid vertices the
    //    rendered linear chords deviate from the smooth field - on curved ground by
    //    enough to bury feet or decals placed at the analytic height. Use for ANYTHING
    //    placed visually on the ground (characters, decals, ray picking), and feed it
    //    to SelectionRingNode3D::setHeightQuery for terrain-conforming rings.
    class HeightfieldMesh : public StandardMesh {
    public:
        using HeightFn = std::function<float(float x, float z)>;

        HeightfieldMesh(std::shared_ptr<Manager::ShaderProgram> baseShader,
                        float halfSpan, int gridCells, HeightFn height);

        // The analytic height field (the function the grid samples).
        [[nodiscard]] float heightAt(float x, float z) const;

        // Height of the RENDERED surface at (x,z) - plane interpolation inside the
        // triangle that actually covers the point (see the class comment).
        [[nodiscard]] float heightAtMesh(float x, float z) const;

        // Field surface normal at (x,z) via central differences with half-step eps.
        [[nodiscard]] glm::vec3 normalAt(float x, float z, float eps = 1.0f) const;

    private:
        HeightFn height;
        std::vector<float> gridHeights; // (cells+1)^2 row-major mesh vertex heights
        int cells = 0;
        float half = 0.0f;
    };
} // Model

#endif // SNAKE3_HEIGHTFIELDMESH_H
