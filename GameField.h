#pragma once
#include "PolygonSprite.h"
#include "CubeSprite.h"
#include "TextSprite.h"
#include "SnakeContainer.h"
#include "Eat.h"
#include "Wall.h"
#include "Radar.h"
#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4

struct sWALL {
    Wall* wall;
    GLuint texture;
};

struct sGAME_BOARD {
    PolygonSprite* gameFieldSprite;
    int width;
    int height;
    int x;
    int y;
};

class GameField
{
public:
	GameField(int width, int height);
	~GameField();
	void Render();
	void Render2D();
	void Proceed(SDL_Event* event);
	void setLeftRight(float* pos);
	void setCamera(Camera* camera);
	bool isPauseOrStop();
    Vector2f getHeadPos();
    SDL_Renderer *renderer{};

protected:
    void generateWalls();
    void resetGame();
    void validateDirection();
    void foodProcess();

protected:
	PolygonSprite* gameFieldSprite;
	PolygonSprite* gameFieldSprite2;
	PolygonSprite* gameFieldSprite3{};
	PolygonSprite* gameFieldSprite4{};
	Camera* camera;
	TextSprite* info;
	CubeSprite* Head;
	SnakeContainer* snake;
	Radar* radar;
	Eat* food;
	vector<sWALL> walls;
	eDIRECTION eDirection, cacheDirection;
	Uint32 next_time{};
	bool changeDirection;
	float* left_right{};
    int width;
    int height;
};
