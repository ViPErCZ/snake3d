#include "ResourceManager.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

namespace Manager {
    ResourceManager::~ResourceManager() {
        Release();
    }

    bool ResourceManager::createTexture(char *aPath, GLint filter) {
//        SDL_Surface *image;
//        image = SDL_LoadBMP(aPath);
//
//        if ( image == nullptr ) {
//            /*char error[255];
//            strcpy(error, SDL_GetError());*/
//            return false;
//        }
//
//        /* Standard OpenGL texture creation code */
//        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
//
//        GLuint texture;
//        glGenTextures(1,&texture);
//        glBindTexture(GL_TEXTURE_2D,texture);
//
//        // select modulate to mix texture with color for shading
//        glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
//
////        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter );
////        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter );
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//        // the texture wraps over at the edges (repeat)
//        /*glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
//        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );*/
//
//        gluBuild2DMipmaps(GL_TEXTURE_2D, 4, image->w,
//                          image->h, GL_BGR_EXT,
//                          GL_UNSIGNED_BYTE, image->pixels);
//
//        SDL_FreeSurface( image );
//        this->textures.push_back(texture);
//
        string filename = string(aPath);

        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << aPath << std::endl;
            stbi_image_free(data);
        }

        this->textures.push_back(textureID);

        return true;
    }

    bool ResourceManager::Release() {
        for (auto Iter = textures.begin(); Iter < textures.end(); Iter++)
            glDeleteTextures(1, &(*Iter));

        return true;
    }

    GLuint ResourceManager::getTexture(int texture) {
        if (texture > 0 && texture < (int)textures.size()) {
            auto Iter = textures.begin() + texture;

            return (*Iter);
        }

        throw std::invalid_argument( "received texture index not exists" );
    }

} // Manager