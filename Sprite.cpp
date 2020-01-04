#include "stdafx.h"
#include "Sprite.h"

Sprite::Sprite():
isVisible(true),
activeTexture(0)
{
	pos.x = pos.y = pos.z = 0;
	zoom.x = zoom.y = 1.0f;
	rotate[0].angle = rotate[0].x = rotate[0].y = rotate[0].z = 0.0f;
	rotate[1].angle = rotate[1].x = rotate[1].y = rotate[1].z = 0.0f;
	rotate[2].angle = rotate[2].x = rotate[2].y = rotate[2].z = 0.0f;
}

Sprite::~Sprite()
= default;

void Sprite::Render() {
	if (isVisible) {
		if (!textures.empty())
			glBindTexture( GL_TEXTURE_2D, getActiveTexture() );

		glLoadIdentity( );
		glTranslatef( pos.x, pos.y, pos.z );
		glScalef(zoom.x, zoom.y, zoom.z);
		glRotatef(rotate[0].angle, rotate[0].x, rotate[0].y, rotate[0].z);
		glRotatef(rotate[1].angle, rotate[1].x, rotate[1].y, rotate[1].z);
		glRotatef(rotate[2].angle, rotate[2].x, rotate[2].y, rotate[2].z);
	}
}

Vector3f Sprite::getPos() {
	return pos;
}

Vector3f Sprite::getZoom() {
	return zoom;
}

sSIZE Sprite::getSize() {
	return size;
}

void Sprite::setPos(Vector3f aPos) {
	pos = aPos;
}

void Sprite::setRotate(Vector4f Xrotate, Vector4f Yrotate, Vector4f Zrotate) {
	rotate[0] = Xrotate;
	rotate[1] = Yrotate;
	rotate[2] = Zrotate;
}

bool Sprite::setTexture(char* aPath, GLint filter) {
	SDL_Surface *image;

	image = SDL_LoadBMP(aPath);
    if ( image == nullptr ) {
		/*char error[255];
		strcpy(error, SDL_GetError());*/
        return false;
    }

	size.width = image->w;
	size.height = image->h;

	/* Standard OpenGL texture creation code */
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	
	GLuint texture;
	glGenTextures(1,&texture);
	glBindTexture(GL_TEXTURE_2D,texture);

	// select modulate to mix texture with color for shading
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	 
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter );

	// the texture wraps over at the edges (repeat)
	/*glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );*/
	 		
	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, image->w,
	   image->h, GL_BGR_EXT,
	   GL_UNSIGNED_BYTE, image->pixels);

	SDL_FreeSurface( image );

	textures.push_back(texture);

	return true;
}

bool Sprite::Release() {
	for (auto Iter = textures.begin(); Iter < textures.end(); Iter++)
		glDeleteTextures(1, &(*Iter));

	return true;
}

void Sprite::setVisibility(bool aVisible) {
	isVisible = aVisible;
}

bool Sprite::getVisibility() {
	return isVisible;
}

void Sprite::setActiveTexture(int active) {
	if (active > 0 && active < (int)textures.size())
		activeTexture = active;
	else
		activeTexture = 0;
}

GLuint Sprite::getActiveTexture() {
	auto Iter = textures.begin()+activeTexture;

	return (*Iter);
}

int Sprite::getActiveTextureNum() {
	return activeTexture;
}

bool Sprite::getIsDown() {
	return false;
}

bool Sprite::getIsHovered() {
	return false;
}

void Sprite::setZoom(Vector3f aZoom) {
	zoom = aZoom;
}

GLuint Sprite::loadTexture(char *aPath, GLint filter) {
    SDL_Surface *image;

    image = SDL_LoadBMP(aPath);
    if ( image == nullptr ) {
        /*char error[255];
        strcpy(error, SDL_GetError());*/
        return false;
    }

    /* Standard OpenGL texture creation code */
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    GLuint texture;
    glGenTextures(1,&texture);
    glBindTexture(GL_TEXTURE_2D,texture);

    // select modulate to mix texture with color for shading
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter );

    // the texture wraps over at the edges (repeat)
    /*glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );*/

    gluBuild2DMipmaps(GL_TEXTURE_2D, 4, image->w,
                      image->h, GL_BGR_EXT,
                      GL_UNSIGNED_BYTE, image->pixels);

    SDL_FreeSurface( image );

    return texture;
}

void Sprite::addTexture(GLuint texture) {
    this->textures.push_back(texture);
}
