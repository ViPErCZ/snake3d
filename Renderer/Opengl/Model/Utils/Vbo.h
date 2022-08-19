#ifndef SNAKE3_VBO_H
#define SNAKE3_VBO_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

using namespace std;

namespace ModelUtils {

    // Structure to standardize the vertices used in the meshes
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 color;
        glm::vec2 texUV;
        glm::vec3 tangents;
        glm::vec2 biTangents;
    };

    class Vbo {
    public:
        // Constructor that generates a Vertex Buffer Object and links it to vertices
        explicit Vbo(vector<Vertex>& vertices);
        // Reference ID of the Vertex Buffer Object
        GLuint ID{};
        // Binds the VBO
        void bind() const;
        // Unbinds the VBO
        void unBind();
        // Deletes the VBO
        void clear();
    };

} // ModelUtils

#endif //SNAKE3_VBO_H
