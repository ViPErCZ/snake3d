#include "VboIndexer.h"

namespace Manager {
    void VboIndexer::indexVBO(vector<glm::vec3> &in_vertices, vector<glm::vec2> &in_uvs, vector<glm::vec3> &in_normals,
                              vector<unsigned int> &out_indices, vector<glm::vec3> &out_vertices,
                              vector<glm::vec2> &out_uvs, vector<glm::vec3> &out_normals) {
        std::map<PackedVertex,unsigned short> VertexToOutIndex;

        // For each input vertex
        for ( unsigned int i=0; i<in_vertices.size(); i++ ){

            PackedVertex packed = {in_vertices[i], in_uvs[i], in_normals[i]};


            // Try to find a similar vertex in out_XXXX
            unsigned short index;
            bool found = getSimilarVertexIndex_fast( packed, VertexToOutIndex, index);

            if ( found ){ // A similar vertex is already in the VBO, use it instead !
                out_indices.push_back( index );
            }else{ // If not, it needs to be added in the output data.
                out_vertices.push_back( in_vertices[i]);
                out_uvs     .push_back( in_uvs[i]);
                out_normals .push_back( in_normals[i]);
                unsigned short newindex = (unsigned short)out_vertices.size() - 1;
                out_indices .push_back( newindex );
                VertexToOutIndex[ packed ] = newindex;
            }
        }
    }

    void
    VboIndexer::indexVBO_TBN(vector<glm::vec3> &in_vertices, vector<glm::vec2> &in_uvs, vector<glm::vec3> &in_normals,
                             vector<glm::vec3> &in_tangents, vector<glm::vec3> &in_bitangents,
                             vector<unsigned int> &out_indices, vector<glm::vec3> &out_vertices,
                             vector<glm::vec2> &out_uvs, vector<glm::vec3> &out_normals,
                             vector<glm::vec3> &out_tangents, vector<glm::vec3> &out_bitangents) {
        // For each input vertex
        for ( unsigned int i=0; i<in_vertices.size(); i++ ){

            // Try to find a similar vertex in out_XXXX
            unsigned short index;
            bool found = getSimilarVertexIndex(in_vertices[i], in_uvs[i], in_normals[i],     out_vertices, out_uvs, out_normals, index);

            if ( found ){ // A similar vertex is already in the VBO, use it instead !
                out_indices.push_back( index );

                // Average the tangents and the bitangents
                out_tangents[index] += in_tangents[i];
                out_bitangents[index] += in_bitangents[i];
            }else{ // If not, it needs to be added in the output data.
                out_vertices.push_back( in_vertices[i]);
                out_uvs     .push_back( in_uvs[i]);
                out_normals .push_back( in_normals[i]);
                out_tangents .push_back( in_tangents[i]);
                out_bitangents .push_back( in_bitangents[i]);
                out_indices .push_back( (unsigned short)out_vertices.size() - 1 );
            }
        }
    }

    bool VboIndexer::getSimilarVertexIndex(glm::vec3 &in_vertex, glm::vec2 &in_uv, glm::vec3 &in_normal,
                                           vector<glm::vec3> &out_vertices, vector<glm::vec2> &out_uvs,
                                           vector<glm::vec3> &out_normals, unsigned short &result) {
        // Lame linear search
        for ( unsigned int i=0; i<out_vertices.size(); i++ ){
            if (
                    is_near( in_vertex.x , out_vertices[i].x ) &&
                    is_near( in_vertex.y , out_vertices[i].y ) &&
                    is_near( in_vertex.z , out_vertices[i].z ) &&
                    is_near( in_uv.x     , out_uvs     [i].x ) &&
                    is_near( in_uv.y     , out_uvs     [i].y ) &&
                    is_near( in_normal.x , out_normals [i].x ) &&
                    is_near( in_normal.y , out_normals [i].y ) &&
                    is_near( in_normal.z , out_normals [i].z )
                    ){
                result = i;
                return true;
            }
        }
        // No other vertex could be used instead.
        // Looks like we'll have to add it to the VBO.
        return false;
    }

    bool VboIndexer::is_near(float v1, float v2) {
        return fabs( v1-v2 ) < 0.01f;
    }

    bool VboIndexer::getSimilarVertexIndex_fast(VboIndexer::PackedVertex &packed,
                                                map<PackedVertex, unsigned short> &VertexToOutIndex,
                                                unsigned short &result) {
        auto it = VertexToOutIndex.find(packed);
        if ( it == VertexToOutIndex.end() ){
            return false;
        }else{
            result = it->second;
            return true;
        }
    }

    void
    VboIndexer::computeTangentBasis(vector<glm::vec3> &vertices, vector<glm::vec2> &uvs, vector<glm::vec3> &normals,
                                    vector<glm::vec3> &tangents, vector<glm::vec3> &bitangents) {
        for (unsigned int i=0; i<vertices.size(); i+=3 ){

            // Shortcuts for vertices
            glm::vec3 & v0 = vertices[i+0];
            glm::vec3 & v1 = vertices[i+1];
            glm::vec3 & v2 = vertices[i+2];

            // Shortcuts for UVs
            glm::vec2 & uv0 = uvs[i+0];
            glm::vec2 & uv1 = uvs[i+1];
            glm::vec2 & uv2 = uvs[i+2];

            // Edges of the triangle : postion delta
            glm::vec3 deltaPos1 = v1-v0;
            glm::vec3 deltaPos2 = v2-v0;

            // UV delta
            glm::vec2 deltaUV1 = uv1-uv0;
            glm::vec2 deltaUV2 = uv2-uv0;

            float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
            glm::vec3 tangent = (deltaPos1 * deltaUV2.y   - deltaPos2 * deltaUV1.y)*r;
            glm::vec3 bitangent = (deltaPos2 * deltaUV1.x   - deltaPos1 * deltaUV2.x)*r;

            // Set the same tangent for all three vertices of the triangle.
            // They will be merged later, in vboindexer.cpp
            tangents.push_back(tangent);
            tangents.push_back(tangent);
            tangents.push_back(tangent);

            // Same thing for binormals
            bitangents.push_back(bitangent);
            bitangents.push_back(bitangent);
            bitangents.push_back(bitangent);

        }

        // See "Going Further"
        for (unsigned int i=0; i<vertices.size(); i+=1 )
        {
            glm::vec3 & n = normals[i];
            glm::vec3 & t = tangents[i];
            glm::vec3 & b = bitangents[i];

            // Gram-Schmidt orthogonalize
            t = glm::normalize(t - n * glm::dot(n, t));

            // Calculate handedness
            if (glm::dot(glm::cross(n, t), b) < 0.0f){
                t = t * -1.0f;
            }

        }
    }
} // Manager