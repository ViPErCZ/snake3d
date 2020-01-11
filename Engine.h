#pragma once
#include "stdafx.h"
#include "GameField.h"
#include "CubeSprite.h"
#include "TTFSprite.h"

#define TICK_INTERVAL 0

class Engine
{
public:
	Engine(SDL_Renderer * renderer, int aWidth, int aHeight);
	~Engine(void);
	void Run(SDL_Window* window);
protected:
	Uint32 TimeLeft(void);
	void setup_opengl(bool freeD = false);
	void calculateFPS();
	void Render();

protected:
	SDL_Event event{};
	int cameraType;
	int width;
	int height;
	bool mouseDownIsAction;
	unsigned int last_time;
	unsigned int current_time{};
	double fps;
	unsigned int frame{};
	GameField* gameField;
	TextSprite* info;
	TTFSprite* info2{};
	float angle;
	float left_right;
	float up_down;
    float zoom;
    int testX;
    int testY;
    int testZ;
    int angleX;
    int angleY;
    int angleZ;
	SDL_Renderer * renderer;
};