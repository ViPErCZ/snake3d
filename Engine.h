#pragma once
#include "stdafx.h"
#include "GameField.h"
#include "CubeSprite.h"
#include "TTFSprite.h"
#include "Camera.h"

#define TICK_INTERVAL 0

class Engine
{
public:
	Engine(SDL_Renderer * renderer, int aWidth, int aHeight);
	~Engine();
	void Run(SDL_Window* window);
protected:
	Uint32 TimeLeft();
	void setup_opengl(bool freeD = false);
	void calculateFPS();
	void Render();

protected:
	SDL_Event event{};
	int width;
	int height;
	bool mouseDownIsAction;
	unsigned int last_time;
	unsigned int current_time{};
	double fps;
	unsigned int frame{};
	GameField* gameField;
	Camera* camera;
	TextSprite* info;
	float angle;
	float left_right;
	float up_down;
    float zoom;
    float angleX;
	SDL_Renderer * renderer;
};