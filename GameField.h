#pragma once
#include "PolygonSprite.h"
#include "CubeSprite.h"
#include "TextSprite.h"
#include "SnakeContainer.h"
#include "Eat.h"
#include "Wall.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4

struct sWALL {
    Wall* wall;
    GLuint texture;
};

class GameField
{
public:
	GameField();
	~GameField();
	void Render();
	void Render2D();
	void Proceed(SDL_Event* event);
	void setLeftRight(float* pos);
    Vector2f getHeadPos();
    SDL_Renderer *renderer;

protected:
    void generateWalls();
    void resetGame();
    void validateDirection();

protected:
	PolygonSprite* gameFieldSprite;
	PolygonSprite* gameFieldSprite2;
	PolygonSprite* gameFieldSprite3;
	PolygonSprite* gameFieldSprite4;
	TextSprite* info;
	CubeSprite* Head;
	SnakeContainer* snake;
	Eat* food;
	vector<sWALL> walls;
	eDIRECTION eDirection, cacheDirection;
	Uint32 next_time;
	bool changeDirection;
	float* left_right;
};
