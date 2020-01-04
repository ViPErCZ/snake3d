#pragma once
#include "CubeSprite.h"
#include <vector>

//#define UNIT_MOVE 32
#define UNIT_MOVE 2
#define CUBE_SIZE 32

using namespace std;

enum eDIRECTION {
	STOP = 0,
	LEFT = 1,
	RIGHT = 2,
	UP = 3,
	DOWN = 4,
	CRASH = 100,
	PAUSE = 200,
};

struct sSNAKE_TILE {
	CubeSprite* tile;
	eDIRECTION direction;
	float alpha;
};

class SnakeContainer
{
public:
	SnakeContainer();
	~SnakeContainer();
	void Init();
	void Release();
	void AddTile(eDIRECTION aDirection);
	void Render();
	void Move(eDIRECTION aDirection);
	Vector2f getHeadPos();
	bool isSnakeInThisPos(vector<sSNAKE_TILE*>::const_iterator myself, Vector2f pos);
	bool isSnakeInThisPos(Vector2f pos);
	Vector2f getFreeArray();
	bool isSnakeCrash();
	bool isChangeDirectionAllowed();
	int getSize();
	void reset();
	bool isSnakeVisible();
    eDIRECTION getHeadDirection();

protected:
	vector<sSNAKE_TILE*> snake;
	GLuint tileTexture;
};
