#pragma once
#include "stdafx.h"
#include <vector>

using namespace std;

struct Vector2f {
	float x, y;
};

struct Vector3f {
	float x, y, z;
};

struct Vector4f {
	float angle, x, y, z;
};

struct sSIZE {
	int width;
	int height;
};

class Sprite
{
public:
	Sprite(void);
	virtual ~Sprite(void);
	virtual void Render();
	Vector3f getPos();
	sSIZE getSize();
	Vector3f getZoom();
	void setPos(Vector3f aPos);
	void setRotate(Vector4f Xrotate, Vector4f Yrotate, Vector4f Zrotate);
	void setZoom(Vector3f aZoom);
	void setVisibility(bool aVisible);
	bool getVisibility();
	bool setTexture(char* aPath, GLint filter = GL_LINEAR_MIPMAP_LINEAR);
	void setActiveTexture(int active);
	GLuint getActiveTexture();
	int getActiveTextureNum();
	virtual bool Release(void);
	virtual bool getIsDown();
	virtual bool getIsHovered();
	static GLuint loadTexture(char* aPath, GLint filter = GL_LINEAR_MIPMAP_LINEAR);
	void addTexture(GLuint texture);
protected:
	Vector3f pos{};
	Vector3f zoom{};
	Vector4f rotate[3]{};
	vector<GLuint> textures;
	bool isVisible;
	sSIZE size{};
	int activeTexture;
};
