#include "stdafx.h"
#include "Eat.h"

Eat::Eat()
= default;

Eat::~Eat()
= default;

void Eat::setPos() {
	srand(time(nullptr));

	int fieldX = rand() % 46 + 1; // 46 poli

	srand(time(nullptr));

	int fieldY = rand() % 23 + 1;

	Vector3f pos = {static_cast<float>(fieldX*32+16), static_cast<float>(fieldY*32+16), 15};

	CubeSprite::setPos(pos);
}

void Eat::setPos(Vector2f pos) {
	Vector3f posSprite = {pos.x, pos.y, 15};

	CubeSprite::setPos(posSprite);
}