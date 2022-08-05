#ifndef SNAKE3_VBOINDEXER_H
#define SNAKE3_VBOINDEXER_H

#include <map>
#include <glm/glm.hpp>
#include <vector>
#include <cstring>

using namespace std;

namespace Manager {

    class VboIndexer {
        struct PackedVertex{
            glm::vec3 position;
            glm::vec2 uv;
            glm::vec3 normal;
            bool operator<(const PackedVertex that) const{
                return memcmp((void*)this, (void*)&that, sizeof(PackedVertex))>0;
            };
        };
    public:
        static void indexVBO(
                std::vector<glm::vec3> & in_vertices,
                std::vector<glm::vec2> & in_uvs,
                std::vector<glm::vec3> & in_normals,

                std::vector<unsigned short> & out_indices,
                std::vector<glm::vec3> & out_vertices,
                std::vector<glm::vec2> & out_uvs,
                std::vector<glm::vec3> & out_normals
        );
        static void indexVBO_TBN(
                std::vector<glm::vec3> & in_vertices,
                std::vector<glm::vec2> & in_uvs,
                std::vector<glm::vec3> & in_normals,
                std::vector<glm::vec3> & in_tangents,
                std::vector<glm::vec3> & in_bitangents,

                std::vector<unsigned short> & out_indices,
                std::vector<glm::vec3> & out_vertices,
                std::vector<glm::vec2> & out_uvs,
                std::vector<glm::vec3> & out_normals,
                std::vector<glm::vec3> & out_tangents,
                std::vector<glm::vec3> & out_bitangents
        );
        static void computeTangentBasis(
                // inputs
                std::vector<glm::vec3> & vertices,
                std::vector<glm::vec2> & uvs,
                std::vector<glm::vec3> & normals,
                // outputs
                std::vector<glm::vec3> & tangents,
                std::vector<glm::vec3> & bitangents
        );
    protected:
        static bool getSimilarVertexIndex(
                glm::vec3 & in_vertex,
                glm::vec2 & in_uv,
                glm::vec3 & in_normal,
                std::vector<glm::vec3> & out_vertices,
                std::vector<glm::vec2> & out_uvs,
                std::vector<glm::vec3> & out_normals,
                unsigned short & result
        );
        static bool is_near(float v1, float v2);
        static bool getSimilarVertexIndex_fast(
                PackedVertex & packed,
                std::map<PackedVertex,unsigned short> & VertexToOutIndex,
                unsigned short & result
        );
    };

} // Manager

#endif //SNAKE3_VBOINDEXER_H
