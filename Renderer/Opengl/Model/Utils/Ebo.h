#ifndef SNAKE3_EBO_H
#define SNAKE3_EBO_H

#include<vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace std;

namespace ModelUtils {

    class Ebo {
    public:
        // Constructor that generates a Elements Buffer Object and links it to indices
        explicit Ebo(vector<GLuint>& indices);
        // ID reference of Elements Buffer Object
        GLuint ID{};
        // Binds the EBO
        void bind();
        // Unbinds the EBO
        void unBind();
        // Deletes the EBO
        void clear();
    };

} // ModelUtils

#endif //SNAKE3_EBO_H
